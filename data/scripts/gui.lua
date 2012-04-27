require("data/scripts/widgets")


local GUI = {}
GUI.__index = GUI

function GUI.create()
	g = {}
	setmetatable(g, GUI)
	g.widgets = {}
	g.thread = NewThread(GUI.update)
	g.visible = false
	return g
end

function GUI:addWidgetByName(name)
	for k, v in pairs( configs.configs_id ) do
		if v.name == name then id = k end
	end
	if id == nil then
		Debug( 6, "Widget with name " .. name .. " not found." )
		return
	end
	self:addWidget(id)
end

function GUI:addWidget(id)
	w = Widget.create(id)
	if not w then return end
	self:registerWidget(w)
end

function GUI:registerWidget(w)
	if not w then return end
	table.insert(self.widgets, w.id, w)
	local t = w:children()
	if not t then return end
	for k, v in pairs(t) do
		self:registerWidget(Widget.createFromExits(v))
	end
end

function GUI:getWidgetByName(name)
	for k,v in pairs(self.widgets) do
		if v.name == name then return v end
	end
end

function GUI:getWidgetById(id)
	for k,v in pairs(self.widgets) do
		if v.cid == id then return v end
	end
end

function GUI:getWidget(id)
	return self.widgets[id]
end

function GUI:show()

end

function GUI:update()
	print("thread\n\n\n")
end


return GUI
