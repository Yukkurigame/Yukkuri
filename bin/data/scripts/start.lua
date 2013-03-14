
require("data/scripts/GUI")
require("data/scripts/environment")

Input.set("bindings_game")
local player = UnitManager.createUnit(constants.utPlayer)
Player = UnitManager.getPlayer()

Camera.rotate(55, 1.0, 0.0, 0.0)
Camera.rotate(45, 0.0, 0.0, 1.0)
Camera.setTarget(player)

GUI:loadGroup("game")

Map.generateRegion(2, "4525")

Map.active(true)
Map.loadRegion('region_test2_881880')



Environment:setup()

UnitManager.createUnit(constants.utPlant, 100, 0)

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
