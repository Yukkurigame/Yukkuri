
gui = require("data/scripts/gui").create()

SetBindings("bindings_game")

Interface.loadWidget("widget_fps_428624")
Interface.loadWidget("widget_time_493245")
local stats = Interface.loadWidget("widget_stats_98072")

local player = UnitManager.createUnit(constants.utPlayer)
SetCameraTarget(player)

LoadMapRegion('region_test2_881880')

local plevel = Interface.getWidgetByName("plevel", stats)
local pdays = Interface.getWidgetByName("pdays", stats)

if plevel then
	plevel.bind(player, "level")
end
if pdays then
	pdays.bind(player, "days")
end


local hpbar = Interface.getWidgetByName("phpbar", stats)
local fedbar = Interface.getWidgetByName("pfedbar", stats)
local expbar = Interface.getWidgetByName("pexpbar", stats)
if hpbar then
	hpbar.bind(player, "hp")
	hpbar.bindBarMax(player, "hpmax")
end
if fedbar then
	fedbar.bind(player, "fed")
	fedbar.setBarSize(100)
end
if expbar then
	expbar.bind(player, "exp")
	expbar.bindBarMax(player, "expmax")
end


gui:show()
