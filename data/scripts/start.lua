
gui = require("data/scripts/gui").create()

SetBindings("bindings_game")

gui:addWidget("widget_fps_428624")
gui:addWidget("widget_time_493245")
gui:addWidgetByName("stats")

local player = UnitManager.createUnit(constants.utPlayer)
SetCameraTarget(player)

LoadMapRegion('region_test2_881880')

local plevel = gui:getWidgetByName("plevel")
local pdays = gui:getWidgetByName("pdays")
if plevel then
	plevel:bind(player, "level")
end
if pdays then
	pdays:bind(player, "days")
end


local hpbar = gui:getWidgetByName("phpbar")
local fedbar = gui:getWidgetByName("pfedbar")
local expbar = gui:getWidgetByName("pexpbar")
if hpbar then
	hpbar:bind(player, "hp")
	hpbar:bindBarMax(player, "hpmax")
end
if fedbar then
	fedbar:bind(player, "fed")
	fedbar:setBarSize(100)
end
if expbar then
	expbar:bind(player, "exp")
	expbar:bindBarMax(player, "expmax")
end


gui:show()
