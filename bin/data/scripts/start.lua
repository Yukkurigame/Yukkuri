
require("data/scripts/3rdparty/bit")

local fps = Interface.createWidget("widget_fps_5452434", constants.wtText, {
	name = "fps", align = bit.bor(constants.waLEFT, constants.waBOTTOM),
	font = "./DejaVuSans.ttf",
	text = "FPS: ", fontcolor = {80,80,80},
	fontsize = 20, textalign = constants.waLEFT,
	y = 0, x = 10
})

fps:bindParam(nil, constants.gbpFPS)


local time = Interface.createWidget("widget_time_493245", constants.wtText, {
	name = "time", image = "sprite_widget_time_768888",
	height = 70, font = "./LiberationSerif-Regular.ttf",
	texty = 8, width = 263, fontsize = 26,
	align = bit.bor(constants.waRIGHT, constants.waTOP),
	bgimage = "misc.png"
})


Input.set("bindings_game")
local player = UnitManager.createUnit(constants.utPlayer)
Player = UnitManager.getPlayer()
Camera.setTarget(player)



local stats = Interface.createWidget("widget_stats_98072", constants.wtBlank, {
		name = "stats", image = "sprite_widget_stats_909956",
		align = bit.bor(constants.waLEFT, constants.waTOP),
		bgposx = 265, bgimage = "misc.png", width = 200, height = 300,
})


local plevel = Interface.addChild(stats, "textwidget_plevel_152271",
	constants.wtText, {
		name = "plevel", font = "./LiberationSerif-Regular.ttf",
		align = bit.bor(constants.waRIGHT, constants.waTOP),
		text = "Level: ", fontsize = 18, textalign = constants.waLEFT,
		y = 15, x = -30 })
if plevel then
	plevel:bindParam(player, constants.uBaseLevel)
end


Map.active(true)
Map.loadRegion('region_test2_881880')

--[[
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
-- ]]--


toggle_interface()


Thread.resume(Thread.newThread(function()
	local spawner = require("data/scripts/spawner"):new()
	while true do
		spawner:process()
		Thread.wait(500)
	end
end))

