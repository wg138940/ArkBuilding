#include "player_building_model.h"

namespace albc
{
	namespace bm
	{
		BuildingBuffDisplay::BuildingBuffDisplay(const Json::Value& json):
			base_buff(json["base"].asInt()),
			buff(json["buff"].asInt())
		{
		}

		PlayerBuildingChar::PlayerBuildingChar(const Json::Value& json):
			char_id(json["charId"].asString()),
			room_slot_id(json["roomSlotId"].asString()),
			last_ap_add_time(json["lastApAddTime"].asInt64()),
			ap(json["ap"].asInt()),
			index(json["index"].asInt()),
			change_scale(json["changeScale"].asInt()),
			work_time(json["workTime"].asInt())
		{
		}

		PlayerBuildingManufacture::PlayerBuildingManufacture(const Json::Value& json):
			state(json_val_as_enum<PlayerRoomState>(json["state"])),
			formula_id(json["formulaId"].asString()),
			remain_sln_cnt(json["remainSolutionCnt"].asInt()),
			output_sln_cnt(json["outputSolutionCnt"].asInt()),
			capacity(json["capacity"].asInt()),
			ap_cost(json["apCost"].asInt()),
			process_point(json["processPoint"].asDouble()),
			last_update_time(json["lastUpdateTime"].asInt64()),
			complete_work_time(json["completeWorkTime"].asInt64())
		{
		}

		TradingOrderBuff::TradingOrderBuff(const Json::Value& json):
			from(json["from"].asString()),
			param(json["param"].asInt())
		{
		}

		TradingBuff::TradingBuff(const Json::Value& json):
			speed(json["speed"].asDouble()),
			limit(json["limit"].asInt())
		{
		}

		PlayerBuildingTrading::PlayerBuildingTrading(const Json::Value& json):
			buff(TradingBuff(json["buff"])),
			state(json_val_as_enum<PlayerRoomState>(json["state"])),
			stock_limit(json["stockLimit"].asInt()),
			display(BuildingBuffDisplay(json["display"]))
		{
			const auto o_type = json["strategy"].asString();
			if (o_type == "O_GOLD")
			{
				order_type = OrderType::GOLD;
			}
			else if (o_type == "O_DIAMOND")
			{
				order_type = OrderType::ORUNDUM;
			}
			else
			{
				assert(false);
			}
		}

		PlayerBuildingLabor::PlayerBuildingLabor(const Json::Value& json):
			buff_speed(json["buffSpeed"].asDouble()),
			value(json["value"].asInt()),
			max_value(json["maxValue"].asInt()),
			process_point(json["ProcessPoint"].asDouble())
		{
		}

		PlayerBuildingRoom::PlayerBuildingRoom(const Json::Value& json)
		{
			manufacture = json_val_as_dictionary<PlayerBuildingManufacture>(json["MANUFACTURE"]);
			trading = json_val_as_dictionary<PlayerBuildingTrading>(json["TRADING"]);
		}

		PlayerBuilding::PlayerBuilding(const Json::Value& json):
			status_labor(json["status"]["labor"]),
			player_building_room(new PlayerBuildingRoom(json["rooms"])),
			chars(json_val_as_dictionary<PlayerBuildingChar*>(json["chars"], json_ctor<PlayerBuildingChar>))
		{
		}
	}
}
