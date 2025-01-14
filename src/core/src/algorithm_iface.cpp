// ReSharper disable StringLiteralTypo
#include "algorithm_iface.h"
#include "algorithm.h"
#include "model_buff_map.h"
#include "util_locale.h"
#include "data_player.h"
#include "util_time.h"
#include "algorithm_iface_params.h"
#include <future>
namespace albc::algorithm::iface
{
void launch_test(const Json::Value &player_data_json, const Json::Value &game_data_json,
                 const AlbcTestConfig &test_config)
{
    switch (test_config.mode)
    {
    case ALBC_TEST_MODE_SEQUENTIAL:
        algorithm::iface::run_sequential_test(player_data_json, game_data_json, test_config);
        break;

    case ALBC_TEST_MODE_PARALLEL:
        algorithm::iface::run_parallel_test(player_data_json, game_data_json, test_config);
        break;

    case ALBC_TEST_MODE_ONCE:
        algorithm::iface::test_once(player_data_json, game_data_json, test_config);
        break;

    default:
        ALBC_UNREACHABLE();
    }
}

void test_once(const Json::Value &player_data_json, const Json::Value &game_data_json,
               const AlbcTestConfig &test_config)
{
    std::shared_ptr<data::building::BuildingData> building_data;
    std::shared_ptr<data::player::PlayerDataModel> player_data;

    const auto orig_log_level = util::GlobalLogConfig::GetLogLevel();
    util::make_defer([orig_log_level]() { util::GlobalLogConfig::SetLogLevel(orig_log_level); });
    util::GlobalLogConfig::SetLogLevel(static_cast<util::LogLevel>(test_config.base_parameters.level));

    const auto buff_map = model::buff::BuffMap::instance();

    {
        auto sc = SCOPE_TIMER_WITH_TRACE("Data feeding");
        LOG_I("Parsing building json object.");
        try
        {
            building_data = std::make_shared<data::building::BuildingData>(game_data_json);
            LOG_I("Loaded ", building_data->chars.size(), " building character definitions.");
            LOG_I("Loaded ", building_data->buffs.size(), " building buff definitions.");
        }
        catch (const std::exception &e)
        {
            LOG_E("Error: Unable to parse game building data json object: ", e.what());
            throw;
        }

        int unsupported_buff_cnt = 0;
        for (const auto &[buff_id, buff] : building_data->buffs)
        {
            if (buff_map->count(buff_id) <= 0)
            {
                if (test_config.show_all_ops)
                {
                    util::VariantPut(std::cout, "\"", buff->buff_id, "\": ", buff->buff_name, ": ",
                               xml::strip_xml_tags(buff->description));
                }
                ++unsupported_buff_cnt;
            }
        }
        if (!test_config.show_all_ops)
        {
            LOG_D(unsupported_buff_cnt,
                  R"( unsupported buff found in building data buff definitions. Add "--all-ops" param to check all.)");
        }

        LOG_I("Parsing player json object.");
        try
        {
            player_data = std::make_shared<data::player::PlayerDataModel>(player_data_json);
            LOG_I("Added ", player_data->troop.chars.size(), " existing character instance");
            LOG_I("Added ", player_data->building.player_building_room.manufacture.size(), " factories.");
            LOG_I("Added ", player_data->building.player_building_room.trading.size(), " trading posts.");
            LOG_I("Player building data parsing completed.");
        }
        catch (std::exception &e)
        {
            LOG_E("Error: Unable to parse player data json object: ", e.what());
            throw;
        }
    }

    GenTestModePlayerData(*player_data, *building_data);
    AlgorithmParams params(*player_data, *building_data);
    const auto sc = SCOPE_TIMER_WITH_TRACE("Solving");
    Vector<model::buff::RoomModel *> all_rooms;
    const auto &manu_rooms = mem::unwrap_ptr_vector(params.GetRoomsOfType(data::building::RoomType::MANUFACTURE));
    const auto &trade_rooms = mem::unwrap_ptr_vector(params.GetRoomsOfType(data::building::RoomType::TRADING));

    all_rooms.insert(all_rooms.end(), manu_rooms.begin(), manu_rooms.end());
    all_rooms.insert(all_rooms.end(), trade_rooms.begin(), trade_rooms.end());

    algorithm::MultiRoomIntegerProgramming alg_all(all_rooms, params.GetOperators(),
                                        test_config.base_parameters.solver_parameters);

    algorithm::AlgorithmResult result;
    alg_all.Run(result);
}

void run_parallel_test(const Json::Value &player_data_json, const Json::Value &game_data_json,
                       const AlbcTestConfig &test_config)
{
    const auto sc = SCOPE_TIMER_WITH_TRACE("Parallel test");

    LOG_I("Running parallel test for ", test_config.param, " concurrency");

    Vector<std::future<void>> futures;
    for (int i = 0; i < test_config.param; ++i)
    {
        futures.push_back(std::async(std::launch::async, test_once, player_data_json, game_data_json, test_config));
    }

    // wait for all threads to finish
    for (auto &f : futures)
    {
        f.wait();
    }

    LOG_I("Parallel test completed.");
}

void run_sequential_test(const Json::Value &player_data_json, const Json::Value &game_data_json,
                         const AlbcTestConfig &test_config)
{
    LOG_I("Running sequential test for ", test_config.param, " iterations");

    for (int i = 0; i < test_config.param; ++i)
    {
        test_once(player_data_json, game_data_json, test_config);
    }

    LOG_I("Sequential test completed.");
}
} // namespace albc::algorithm::iface
