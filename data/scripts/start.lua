
Interface.loadWidget("widget_fps_428624")
Interface.loadWidget("widget_time_493245")

SetBindings("bindings_game")

local stats = Interface.loadWidget("widget_stats_98072")

local player = UnitManager.createUnit(constants.utPlayer)
Camera.setTarget(player)

LoadMapRegion('region_test2_881880')

local plevel = Interface.getWidgetByName("plevel", stats)
local pdays = Interface.getWidgetByName("pdays", stats)
if plevel then
	plevel:bindParam(player, constants.uBaseLevel)
end
if pdays then
	pdays:bindParam(player, constants.uBaseAge)
end


local hpbar = Interface.getWidgetByName("phpbar", stats)
local fedbar = Interface.getWidgetByName("pfedbar", stats)
local expbar = Interface.getWidgetByName("pexpbar", stats)
if hpbar then
	hpbar:bindParam(player, constants.uStateHP)
	hpbar:bindBarMaxValue(player, constants.uParamHP)
end
if fedbar then
	fedbar:bindParam(player, constants.uStateFed)
	fedbar:BarSize(100)
end
if expbar then
	expbar:bindParam(player, constants.uStateExp)
	expbar:bindBarMaxValue(player, constants.uParamExp)
end

toggle_interface()

Thread.resume(Thread.newThread(function()
	local spawner = require("data/scripts/spawner"):new()
	while true do
		spawner:process()
		Thread.wait(500)
	end
end))
