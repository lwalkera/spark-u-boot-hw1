typedef enum
{
    FAST_BUS,
    ASYNC_BUS,
    SYNC_BUS
} BusMode;

typedef enum
{
    CLK_EXT,
    CLK_75,
    CLK_100,
    CLK_125,
    CLK_150,
    CLK_166,
    CLK_180,
    CLK_200,
    CLK_210,
    CLK_222,
    CLK_225,
    CLK_250,
    CLK_300,
    CLK_325,
    CLK_350,
    CLK_368,
    CLK_375,
    CLK_400,
    CLK_425,
    CLK_442,
    CLK_450,
    CLK_475,
    CLK_490,
    CLK_500,
    CLK_NOCHANGE

} ClockRates;

typedef enum
{
    FCLK_DIV_1=0,
    FCLK_DIV_2=1,
    FCLK_DIV_4=2,
    FCLK_DIV_8=3,
    FCLK_DIV_16=4,
    FCLK_NOCHANGE

} FClockDivide;

typedef enum
{
    HCLK_DIV_1,
    HCLK_DIV_2,
    HCLK_DIV_4,
    HCLK_DIV_5,
    HCLK_DIV_6,
    HCLK_DIV_8,
    HCLK_DIV_16,
    HCLK_DIV_32,
    HCLK_NOCHANGE
} HClockDivide;

typedef enum
{

    PCLK_DIV_1,
    PCLK_DIV_2,
    PCLK_DIV_4,
    PCLK_DIV_8,
    PCLK_NOCHANGE
} PClockDivide;
