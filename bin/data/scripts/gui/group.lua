
require("data/scripts/utils")

WidgetGroup = class({
	widgets = {}
})

function WidgetGroup:toggle()
	for index, value in ipairs(widgets) do
		value:toggle()
	end
end

function WidgetGroup:add(widget)
	table.insert(self.widgets, widget)
end

return WidgetGroup