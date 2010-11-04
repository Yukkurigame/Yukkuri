LoadAllConfigs("tiles")
LoadAllConfigs("map")
LoadAllConfigs("widget")
LoadAllConfigs("entity")

gui = require("data/scripts/gui").create()

gui:addWidget("fps")
gui:addWidget("time")
gui:addWidget("pstats")

CreateUnit(1)

local plevel = gui:GetWidgetByName("plevel")
local pdays = gui:GetWidgetByName("pdays")
if plevel then
	plevel:bind(1, "level")
end
if pdays then
	pdays:bind(1, "days")
end


local hpbar = gui:GetWidgetByName("phpbar")
local fedbar = gui:GetWidgetByName("pfedbar")
local expbar = gui:GetWidgetByName("pexpbar")
if hpbar then
	hpbar:bind(1, "hp")
	hpbar:bindBarMax(1, "hpmax")
end
if fedbar then
	fedbar:bind(1, "fed")
	fedbar:setBarSize(100)
end
if expbar then
	expbar:bind(1, "exp")
	expbar:bindBarMax(1, "expmax")
end


GUI:show()