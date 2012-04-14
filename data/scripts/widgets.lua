
Widget = {}
Widget.__index = Widget

function Widget.create(name)
	local w = {}
	setmetatable(w, Widget)
	if w:load(name) then
		return w
	end
	Debug(2, "Widget " .. name .. " creation failed")
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

function Widget:load(name)
	if name == nil then return end
	self.name = name
	self.id = LoadWidget(name)
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
