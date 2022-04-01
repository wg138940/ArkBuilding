// ALBC API commons
// Language: C
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

enum AlbcLogLevel
{
    ALBC_LOG_LEVEL_DEBUG = 0,   // DEBUG 等级，输出所有日志
    ALBC_LOG_LEVEL_INFO = 1,
    ALBC_LOG_LEVEL_WARN = 2,
    ALBC_LOG_LEVEL_ERROR = 3
};

enum AlbcTestMode
{
    ALBC_TEST_MODE_ONCE = 0,
    ALBC_TEST_MODE_SEQUENTIAL = 1,
    ALBC_TEST_MODE_PARALLEL = 2
};

typedef struct
{
    bool gen_lp_file;
    bool gen_all_solution_details;
    double solve_time_limit;
    double model_time_limit;
} AlbcSolverParameters;

typedef struct
{
    AlbcSolverParameters solver_parameters;
    AlbcLogLevel level;
} AlbcParameters;

typedef struct
{
    AlbcParameters base_parameters;
    AlbcTestMode mode;
    int param;
    bool show_all_ops;
} AlbcTestConfig;

typedef struct
{
    const char *what;
    int code;
} AlbcException;

enum AlbcRoomType
{
    ALBC_ROOM_NONE = 0,
    ALBC_ROOM_CONTROL = 1 << 0,
    ALBC_ROOM_POWER = 1 << 1,
    ALBC_ROOM_MANUFACTURE = 1 << 2,
    ALBC_ROOM_SHOP = 1 << 3,
    ALBC_ROOM_DORMITORY = 1 << 4,
    ALBC_ROOM_MEETING = 1 << 5,
    ALBC_ROOM_HIRE = 1 << 6,
    ALBC_ROOM_ELEVATOR = 1 << 7,
    ALBC_ROOM_CORRIDOR = 1 << 8,
    ALBC_ROOM_TRADING = 1 << 9,
    ALBC_ROOM_WORKSHOP = 1 << 10,
    ALBC_ROOM_TRAINING = 1 << 11,
    ALBC_ROOM_FUNCTIONAL = 0b111001111110,
    ALBC_ROOM_ALL = (1 << 12) - 1
};

enum AlbcRoomProductType
{
    ALBC_ROOM_PROD_NONE = 0,
    ALBC_ROOM_PROD_RECORD_1 = 1,
    ALBC_ROOM_PROD_RECORD_2 = 2,
    ALBC_ROOM_PROD_RECORD_3 = 3,
    ALBC_ROOM_PROD_GOLD = 4,
    ALBC_ROOM_PROD_CHIP_1 = 5,
    ALBC_ROOM_PROD_CHIP_2 = 6,
    ALBC_ROOM_PROD_CHIP_3 = 7,
    ALBC_ROOM_PROD_CHIP_4 = 8,
    ALBC_ROOM_PROD_CHIP_5 = 9,
    ALBC_ROOM_PROD_CHIP_6 = 10,
    ALBC_ROOM_PROD_CHIP_7 = 11,
    ALBC_ROOM_PROD_CHIP_8 = 12,
    ALBC_ROOM_PROD_ORIGINIUM_SHARD_ORIROCK = 13,
    ALBC_ROOM_PROD_ORIGINIUM_SHARD_DEVICE = 14,
};

enum AlbcRoomOrderType
{
    ALBC_ROOM_ORDER_NONE = 0,
    ALBC_ROOM_ORDER_GOLD = 1,
    ALBC_ROOM_ORDER_ORUNDUM = 2
};

// log callback
typedef void (*AlbcLogCallback)(const char *);
typedef void (*AlbcFlushLogCallback)();

#ifdef __cplusplus
}
#endif