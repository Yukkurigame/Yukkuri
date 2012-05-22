

local Spawner = {}
Spawner.__index = Spawner

function Spawner.new()
	g = {}
	setmetatable(g, Spawner)
	g.items = {}
	g.max_spawn = configs:get( 'max_spawn', 'config_general', 'config' )
	g.max_edibles = configs:get( 'max_edibles', 'config_general', 'config' )
	return g
end

function Spawner:spawn(type)
	if type == nil then
		return
	end
	UnitManager.createUnit(type)
end

function Spawner:process()
	if UnitManager.getUnitsSize(constants.utEntity) < self.max_spawn then
		self:spawn(constants.utEntity)
	end
	if UnitManager.getUnitsSize(constants.utPlant) < self.max_edibles then
		self:spawn(constants.utPlant)
	end
end

return Spawner