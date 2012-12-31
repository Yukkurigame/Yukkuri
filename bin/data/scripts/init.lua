
math.randomseed(os.time())

-- Require all libraries needed in configs
-- require("data/scripts/debugger")()
require("data/scripts/console")

-- Load configs
configs = require("data/scripts/configs").new()
configs:loadAll("config")
configs:loadAll("sprites")
configs:loadAll("tiles")
configs:loadAll("map")
configs:loadAll("widget")
configs:loadAll("entity")
