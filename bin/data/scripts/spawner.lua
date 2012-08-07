

local Spawner = {}
Spawner.__index = Spawner

function Spawner.new()
	g = {}
	setmetatable(g, Spawner)
	g.items = {}
	g.max_spawn = configs:get( 'max_spawn', 'config_general', 'config' )
	g.max_edibles = configs:get( 'max_edibles', 'config_general', 'config' )
	g.width = configs:get( 'windows_width', 'config_general', 'config' )
	g.halfWidth = g.width / 2
	g.height = configs:get( 'windows_height', 'config_general', 'config' ) / 2
	g.halfHeight = g.height / 2
	return g
end

function Spawner:spawn(type, x, y, proto)
	if type == nil then
		return
	end
	UnitManager.createUnit(type, x, y, proto)
end

function neg()
	--- Lol, not sure it is faster
	return math.sin( 4.71238898 - 3.14159265 * math.random( 0, 1 ) )
end

function Spawner:process()
	local view = Camera.position()
	if UnitManager.getUnitsSize(constants.utEntity) < self.max_spawn then
		self:spawn(constants.utEntity,
			view.x + ( math.random( self.halfWidth, self.width ) * neg() ),
			view.y + math.random( self.halfHeight, self.height ) * neg(),
			'unit_entity' )
	end
	if UnitManager.getUnitsSize(constants.utPlant) < self.max_edibles then
		self:spawn(constants.utPlant,
		view.x + math.random( self.halfWidth,  self.width ) * neg(),
		view.y + math.random( self.halfHeight,  self.height ) * neg() )
	end
end

return Spawner
