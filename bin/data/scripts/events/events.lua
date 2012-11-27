
require("data/scripts/utils")

Events = class({})

function Events:call(object, string)
	local func = self[string]
	if func ~= nil then
		func(self, object)
	else
		Debug.print(Debug.SCRIPT, "Bad event name: '" .. string .. "' for object '" .. tostring(object) .. "'.")
	end
end
