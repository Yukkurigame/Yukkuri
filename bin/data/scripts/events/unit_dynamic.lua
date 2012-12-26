
require("data/scripts/events/events")

UnitDynamic = class(Events)


function UnitDynamic:attack(unit)
	local closest = unit:getClosest(constants.utDynamic, 120.0)
	if closest ~= nil then
		self:hit(closest, unit)
	end
	-- Attack animation
end


function UnitDynamic:hit(unit, attacker)
	unit:setAction("hit")
	local ubuild = unit:getBuild()
	local abuild = attacker:getBuild()
	-- ubuild.
	-- Attack animation
end