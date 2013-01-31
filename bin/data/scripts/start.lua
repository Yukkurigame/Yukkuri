
require("data/scripts/GUI")
require("data/scripts/environment")

Input.set("bindings_game")
local player = UnitManager.createUnit(constants.utPlayer)
Player = UnitManager.getPlayer()
Camera.setTarget(player)
GUI:loadGroup("game")

Map.active(true)
Map.loadRegion('region_test2_881880')

Environment:setup()


toggle_interface()

Thread.resume(Thread.newThread(function()
	local spawner = require("data/scripts/spawner"):new()
	while true do
		spawner:process()
		Thread.wait(500)
	end
end))
