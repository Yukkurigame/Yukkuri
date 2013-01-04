
require("data/scripts/events/unit_dynamic")

UnitEntity = class(UnitDynamic)


function UnitEntity:action(unit)
	self:super():action(unit)

	-- Check if under attack
	local attaker = unit:storage().lasthit
	if attaker and attaker:object_present() then
		local dist = unit:dist(attaker)
		if dist > 1000 then
			unit:storage().lasthit = nil
		else
			local cb = unit:getBuild()
			local php = cb:get(constants.uParamHP)
			local shp = cb:get(constants.uStateHP)
			--if unit:Moving() == 0 then
				local apos = attaker:UnitPos()
				if shp / php < 0.2 then
					unit:addPathTarget(-apos.x, -apos.y, 9)
				elseif dist > 30 then
					unit:addPathTarget(apos.x, apos.y, 5)
				end
			--end
		end
		if dist < 120 then
			self:attack(unit, attaker)
		end
	else
		unit:storage().lasthit = nil
	end

	if unit:Moving() == 0 then
		local x = unit:UnitX() + math.random(-150, 150)
		local y = unit:UnitY() - math.random(-150, 150)
		unit:addPathTarget(x, y)
	end
end

function UnitEntity:goto(unit, target)

end