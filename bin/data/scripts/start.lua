
require("data/scripts/gui/GUI")
require("data/scripts/environment")

Input.set("bindings_game")
local player = UnitManager.createUnit(constants.utPlayer, 1000, 1000)
Player = UnitManager.getPlayer()

Camera.rotate(55, 1.0, 0.0, 0.0)
Camera.rotate(45, 0.0, 0.0, 1.0)
Camera.setTarget(player)

Map.active(true)
--if Map.loadRegion('') <= 0 then
	Map.generateRegion(1, "85882")
	Map.loadRegion('')
--end

GUI:loadGroup("game")


Environment:setup()

plight = Lights.create(constants.ltPoint)
if plight ~= nil then
	plight:color({1,1,1})
	plight:diffuse(100)
	plight:position({10, 10, 0.0})
end


toggle_interface()

Thread.resume(Thread.newThread(function()
	local spawner = require("data/scripts/spawner"):new()
	while true do
		spawner:process()
		Thread.wait(500)
	end
end))
