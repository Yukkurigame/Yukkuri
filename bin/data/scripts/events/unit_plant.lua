
require("data/scripts/events/unit_dynamic")

UnitPlant = class(UnitDynamic)

function UnitPlant:die(unit)
	unit:Deleted(true)
end