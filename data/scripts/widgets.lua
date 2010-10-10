
Widget = {}
Widget.__index = Widget

function Widget.create(name)
	local w = {}
	setmetatable(w, Widget)
	w:load(name)
	return w
end

function Widget:load(name)
	if name == nil then return end
	self.id = LoadWidget(name)
	--- Add debug info here
end

function Widget:bind(UnitId, parameter)
	return BindWidget(self.id, UnitId, parameter)
end

function Widget:children( )
	t = WidgetChildren(self.id)
	return t
end