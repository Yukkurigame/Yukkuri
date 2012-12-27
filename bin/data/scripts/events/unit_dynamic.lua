
require("data/scripts/events/events")

UnitDynamic = class(Events)


function UnitDynamic:attack(unit)
	local closest = unit:getClosest(constants.utDynamic, 120.0)
	unit:getBuild():tire(0.1)
	if closest ~= nil then
		closest.storage['attacker'] = unit
		self:hit(closest, unit)
	end
	-- Attack animation
end


function UnitDynamic:hit(unit, attacker)
	unit:setAction("hit")
	unit.storage['lasthit'] = attacker
	if attacker ~= nil then
		local damage = 0
		if type(attacker) == "userdata" then
			local abuild = attacker:getBuild()
			damage = abuild:Damage()
		elseif type(attacker) == "number" then
			damage = attacker
		end
		local ubuild = unit:getBuild()
		ubuild:Damage( damage )
	end
end


function UnitDynamic:die(unit)
	local x = unit:UnitX()
	local y = unit:UnitY()
	local corpse = UnitManager.getUnit(UnitManager.createUnit(
		constants.utCorpse, x, y, 'unit_corpse'))
	corpse:UnitSize(unit:UnitSize())
	corpse:UnitZ(unit:UnitZ() - 0.01)

	local cbuild = corpse:getBuild()
	local ubuild = unit:getBuild()
	cbuild:set(constants.uParamHP,
		ubuild:get(constants.uParamHP) * ubuild:get(constants.uParamFed) / 100)

	if unit.storage['lasthit'] then
		local lbuild = unit.storage['lasthit']:getBuild()
		lbuild:set(constants.uStateExp, lbuild:get(constants.uStateExp) +
			ubuild:get(constants.uParamHP) / ubuild:get(constants.uBaseLevel))
		lbuild:set(constants.uBaseKills, lbuild:get(constants.uBaseKills) + 1)
	end

	unit:Deleted(true)
end
