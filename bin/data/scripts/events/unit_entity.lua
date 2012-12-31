
require("data/scripts/events/unit_dynamic")

UnitEntity = class(UnitDynamic)


function UnitEntity:action(unit)
	self:super():action(unit)

	if unit:Moving() == 0 then
		local x = unit:UnitX() + math.random(-150, 150)
		local y = unit:UnitY() - math.random(-150, 150)
		unit:addPathTarget(x, y)
	end

	-- Check if under attack
	local attaker = unit.storage.lasthit
	if attaker then
		local dist = unit:dist(attaker)
		if dist > 1000 then
			unit.storage.lasthit = nil
		end
	end
end

function UnitEntity:goto(unit, target)

end