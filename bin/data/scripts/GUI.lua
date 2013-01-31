
require("data/scripts/3rdparty/bit")

GUI = { _widgets = {} }
GUI.__index = GUI

function GUI:loadGroup(name)
	local func = nil
	if name ~= nil then
		func = self["widgets_" .. name]
	end
	if type(func) == "function" then
		func(self, char)
	end
end


function GUI:widgets_game()
	-- Creation
	self:getWidget('fps', nil, "widget_fps_5452434", constants.wtText, {
		name = "fps", align = bit.bor(constants.waLEFT, constants.waBOTTOM),
		y = 0, x = 10,
		text = {
			base = "FPS: ", align = constants.waLEFT,
			face = "./DejaVuSans.ttf", size = 20, color = {80,80,80},
	}})
	self:getWidget('time', nil, "widget_time_493245", constants.wtText, {
		name = "time", image = "sprite_widget_time_768888", width = 263,
		height = 70, align = bit.bor(constants.waRIGHT, constants.waTOP),
		text = { y = 8, face = "./LiberationSerif-Regular.ttf", size = 26, }
	})
	self:getWidget('stats', nil, "widget_stats_98072", constants.wtBlank, {
		name = "stats", image = "sprite_widget_stats_909956",
		align = bit.bor(constants.waLEFT, constants.waTOP),
		bgposx = 265, bgimage = "misc.png", width = 200, height = 300,
	})
	self:getWidget('plevel', 'stats', "textwidget_plevel_152271", constants.wtText, {
		name = "plevel", y = 15, x = -30,
		align = bit.bor(constants.waRIGHT, constants.waTOP),
		text = { size = 18, align = constants.waLEFT,
			face = "./LiberationSerif-Regular.ttf", base = "Level: ", },
	})
	self:getWidget('pdays', 'stats', "textwidget_pdays_353161", constants.wtText, {
		name = "pdays", x = -30, y = 45,
		align = bit.bor(constants.waRIGHT, constants.waTOP),
		text = {
			face = "./LiberationSerif-Regular.ttf", base = "Age: ",
			size = 18, align = constants.waLEFT,
		}
	})
	local hpbar = self:getWidget('hpbar', 'stats', "barwidget_phpbar_121678",
		constants.wtBar, {
		name = "phpbar", width = 146, height = 20, y = 80, align = constants.waTOP,
		text = { base = "HP: ", size = 18, align = constants.waLEFT,
			face = "./LiberationSerif-Regular.ttf",
		},
		bar = { color = {255,0,0}, width = 133, height = 8, y = -5 },
		top = { x = -7, y = -5, width = 145, height = 17,
				image = "sprite_bar_cover_693493" }
	})
	local fedbar = self:getWidget('fedbar', 'stats', "barwidget_pfedbar_655231",
		constants.wtBar, {
		name = "pfedbar", width = 146, height = 20, y = 125, align = constants.waTOP,
		text = { base = "Fed: ", size = 18, align = constants.waLEFT,
			face = "./LiberationSerif-Regular.ttf",
		},
		bar = { color = {0,0,255}, width = 133, height = 8, y = -5 },
		top = { x = -7, y = -5, width = 145, height = 17, image = "sprite_bar_cover_693493" }
	})
	local expbar = self:getWidget('expbar', 'stats', "barwidget_pexpbar_361877",
		constants.wtBar, {
		name = "pfedbar", width = 146, height = 20, y = 170, align = constants.waTOP,
		text = { base = "Exp: ", size = 18, align = constants.waLEFT,
			face = "./LiberationSerif-Regular.ttf",
		},
		bar = { color = {0,255,0}, width = 133, height = 8, y = -5 },
		top = { x = -7, y = -5, width = 145, height = 17, image = "sprite_bar_cover_693493" }
	})


	-- Bindings
	self:bindWidget('fps', nil, constants.gbpFPS)
	self:bindWidget('plevel', Player, constants.uBaseLevel)
	self:bindWidget('pdays', Player, constants.uBaseAge)
	self:bindWidget('hpbar', Player, constants.uStateHP)
	self:bindWidget('fedbar', Player, constants.uStateFed)
	self:bindWidget('expbar', Player, constants.uStateExp)

	hpbar:bindBarMaxValue(Player, constants.uParamHP)
	fedbar:BarSize(100)
	expbar:bindBarMaxValue(Player, constants.uParamExp)
end


function GUI:getWidget(name, parent, wname, wtype, wparams)
	if self._widgets[name] == nil and (wname and wtype and wparams) then
		local parent = self._widgets[parent]
		if parent ~= nil then
			self._widgets[name] = Interface.addChild(parent, wname, wtype, wparams)
		else
			self._widgets[name] = Interface.createWidget(wname, wtype, wparams)
		end
	end
	return self._widgets[name]
end


function GUI:bindWidget(name, target, location)
	if self._widgets[name] ~= nil then
		self._widgets[name]:bindParam(target, location)
	end
end

