
require("data/scripts/events/events")

UnitDynamic = class(Events)


function UnitDynamic:init(unit)
	self:super( ):init(unit)
	unit.storage.foodTypes = bit.bor(constants.utPlant, constants.utCorpse)
end


function UnitDynamic:action(unit)
	local ubuild = unit:getBuild()
	ubuild:tire()
end


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


function UnitDynamic:eat(unit)
	local closest = unit:getClosest(unit.storage.foodTypes, 120.0)
	unit:getBuild():tire(0.1)
	if closest ~= nil then
		self:consume(unit, closest)
	end
	-- Attack animation

end


function UnitDynamic:consume(unit, victim)
	local ubuild = unit:getBuild()
	local vbuild = victim:getBuild()
	local dmg = ubuild:Damage()
	local php = ubuild:get(constants.uParamHP)
	local sthp = ubuild:get(constants.uStateHP)
	local pfed = ubuild:get(constants.uParamFed)
	local stfed = ubuild:get(constants.uStateFed)
	local level = ubuild:get(constants.uBaseLevel)

	-- Set hp
	if stfed >= 80 and stfed < 100 and sthp < php then
		sthp = math.min(sthp + dmg / level, php)
		ubuild:set(constants.uStateHP, sthp)
	end

	-- Set fed
	if stfed >= 100 then
		ubuild:set(constants.uStateFed, 100)
		sthp = math.min(sthp + dmg, php)
		ubuild:set(constants.uStateHP, sthp)
	else
		local n = vbuild:get(constants.uChrNutritive)
		stfed = math.min(stfed + dmg * n / level, 100)
		ubuild:set(constants.uStateFed, stfed)
	end

	-- Damage meal
	vbuild:Damage(dmg)
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
