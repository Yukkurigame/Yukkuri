
require("data/scripts/3rdparty/bit")

local fps = Interface.createWidget("widget_fps_5452434", constants.wtText, {
	name = "fps", align = bit.bor(constants.waLEFT, constants.waBOTTOM),
	y = 0, x = 10,
	text = {
		base = "FPS: ", align = constants.waLEFT,
		face = "./DejaVuSans.ttf", size = 20, color = {80,80,80},
	}
})

fps:bindParam(nil, constants.gbpFPS)


local time = Interface.createWidget("widget_time_493245", constants.wtText, {
	name = "time", image = "sprite_widget_time_768888", width = 263,
	height = 70, align = bit.bor(constants.waRIGHT, constants.waTOP),
	text = { y = 8, face = "./LiberationSerif-Regular.ttf", size = 26, }
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
		name = "plevel", y = 15, x = -30,
		align = bit.bor(constants.waRIGHT, constants.waTOP),
		text = { size = 18, align = constants.waLEFT,
			face = "./LiberationSerif-Regular.ttf", base = "Level: ", },
})
if plevel then
	plevel:bindParam(player, constants.uBaseLevel)
end
local pdays = Interface.addChild(stats, "textwidget_pdays_353161",
	constants.wtText, {
		name = "pdays", x = -30, y = 45,
		align = bit.bor(constants.waRIGHT, constants.waTOP),
		text = {
			face = "./LiberationSerif-Regular.ttf", base = "Age: ",
			size = 18, align = constants.waLEFT,
		}
})
if pdays then
	pdays:bindParam(player, constants.uBaseAge)
end

local hpbar = Interface.addChild(stats, "barwidget_phpbar_121678",
	constants.wtBar, {
		name = "phpbar", width = 146, height = 20, y = 80, align = constants.waTOP,
		text = { base = "HP: ", size = 18, align = constants.waLEFT,
			face = "./LiberationSerif-Regular.ttf",
		},
		bar = { color = {255}, width = 133, height = 8, y = -5 },
		top = { x = -7, y = -5, width = 145, height = 17,
				image = "sprite_bar_cover_693493" }
})
if hpbar then
	hpbar:bindParam(player, constants.uStateHP)
	hpbar:bindBarMaxValue(player, constants.uParamHP)
end
local fedbar = Interface.addChild(stats, "barwidget_pfedbar_655231",
	constants.wtBar, {
		name = "pfedbar", width = 146, height = 20, y = 125, align = constants.waTOP,
		text = { base = "Fed: ", size = 18, align = constants.waLEFT,
			face = "./LiberationSerif-Regular.ttf",
		},
		bar = { color = {0,0,255}, width = 133, height = 8, y = -5 },
		top = { x = -7, y = -5, width = 145, height = 17,
				image = "sprite_bar_cover_693493" }
})
if fedbar then
	fedbar:bindParam(player, constants.uStateFed)
	fedbar:BarSize(100)
end
local expbar = Interface.addChild(stats, "barwidget_pexpbar_361877",
	constants.wtBar, {
		name = "pfedbar", width = 146, height = 20, y = 170, align = constants.waTOP,
		text = { base = "Exp: ", size = 18, align = constants.waLEFT,
			face = "./LiberationSerif-Regular.ttf",
		},
		bar = { color = {0,255}, width = 133, height = 8, y = -5 },
		top = { x = -7, y = -5, width = 145, height = 17,
				image = "sprite_bar_cover_693493" }
})
if expbar then
	expbar:bindParam(player, constants.uStateExp)
	expbar:bindBarMaxValue(player, constants.uParamExp)
end



Map.active(true)
Map.loadRegion('region_test2_881880')

toggle_interface()

Thread.resume(Thread.newThread(function()
	local spawner = require("data/scripts/spawner"):new()
	while true do
		spawner:process()
		Thread.wait(500)
	end
end))
