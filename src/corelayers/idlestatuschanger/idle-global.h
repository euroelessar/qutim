#define AWAY_DEF_SECS 180 // 3 mins
#define NA_DEF_SECS 600 // 10 mins
#define AA_CONFIG_GROUP "auto-away"

namespace Core{
class IdleStatusChanger;
};

extern Core::IdleStatusChanger* pIdleStatusChanger;
