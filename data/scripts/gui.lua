require("data/scripts/widgets")

GUI = {}
GUI.__index = GUI

function GUI.create()
	g = {}
	setmetatable(g, GUI)
	g.widgets = {}
	g.thread = NewThread(GUI.update)
	g.visible = false	
	return g
end

function GUI:addWidget(name)
	w = Widget.create(name)
	if not w then return end
	self:RegisterWidget(w)
end

function GUI:RegisterWidget(w)
	if not w then return end
	table.insert(self.widgets, w.id, w)
	local t = w:children()
	if not t then return end
	for k, v in pairs(t) do
		self:RegisterWidget(Widget.createFromExits(v))
	end
end

function GUI:GetWidgetByName(name)
	for k,v in pairs(self.widgets) do
		if v.name == name then return v end
	end
end

function GUI:show()

end

function GUI:update()
	print("thread\n\n\n")
end


return GUI