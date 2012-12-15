
require("data/scripts/3rdparty/bit")

local fps = Interface.createWidget("widget_fps_5452434", constants.wtText)
fps:load({
	name = "fps", align = bit.bor(constants.waLEFT, constants.waBOTTOM),
	font = "./DejaVuSans.ttf",
	text = "FPS: ", fontcolor = {80,80,80},
	fontsize = 20, textalign = constants.waLEFT,
	y = 0, x = 10
})

fps:bindParam(nil, constants.gbpFPS)


local time = Interface.createWidget("widget_time_493245", constants.wtText)
time:load({
	name = "time", image = "sprite_widget_time_768888",
	height = 70, font = "./LiberationSerif-Regular.ttf",
	texty = 8, width = 263, fontsize = 26,
	align = bit.bor(constants.waRIGHT, constants.waTOP),
	bgimage = "misc.png"
})



local console = Interface.createWidget("widget_console", constants.wtText)
console:load({
		name = "console", font = "./LiberationSerif-Regular.ttf",
		align = bit.bor(constants.waLEFT, constants.waTOP),
		textalign = bit.bor(constants.waLEFT, constants.waTOP),
		fontcolor = {255,255,255}, height = 200, width = 300,
		depth = 2, fontsize = 18, bgcolor = {0, 0, 0, 127},
		textx = 5, texty = -5, lineheight = 0.75
})
console:resize(configs:getById('windows_width', 'config_general') - 1, 300)
console:toggle()



Input.set("bindings_game")


local stats = Interface.loadWidget("widget_stats_98072")

local player = UnitManager.createUnit(constants.utPlayer)
Player = UnitManager.getPlayer()
Camera.setTarget(player)

Map.active(true)
Map.loadRegion('region_test2_881880')


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

