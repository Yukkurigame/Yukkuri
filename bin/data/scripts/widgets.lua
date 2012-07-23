
Widget = {}
Widget.__index = Widget

function Widget.create(id)
	local w = {}
	setmetatable(w, Widget)
	if w:load(id) then
		return w
	end
	Debug(2, "Widget " .. id .. " creation failed")
	return nil
end

function Widget.createFromExits(id)
	local w = {}
	setmetatable(w, Widget)
	name = GetWidgetName(id)
	if name then
		w.id = id
		w.name = name
		return w
	end
	Debug(2, "Widget creation from " .. id .. " failed")
	return nil
end

function Widget:load(id)
	if id == nil then return end
	self.cid = id
	self.id = LoadWidget(id)
	self.name = GetWidgetName(self.id)
	if self.id then
		if self.id > 0 then return true end
	end
	return false
end

function Widget:bind(UnitId, parameter)
	return BindWidget(self.id, UnitId, parameter)
end

function Widget:bindBarMax(UnitId, parameter)
	return BindWidgetMaxBar(self.id, UnitId, parameter)
end

function Widget:setBarSize(value)
	WidgetSetBarSize(self.id, value)
end

function Widget:children( )
	t = WidgetChildren(self.id)
	return t
end

function Widget:show()
	ToggleWidget(self.id, true)
end

function Widget:hide()
	ToggleWidget(self.id, false)
end

function Widget:toggle()
	ToggleWidget(self.id)
end
