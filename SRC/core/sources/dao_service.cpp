#include "dao_service.h"

ServiceDAO::ServiceDAO()
{
  id = 0;
  name = "";
  repetition = false;
  initHour = 0;
  endHour = 0;
  group = 0;
  lastServed = 0;
}

ServiceDAO::~ServiceDAO() {}
