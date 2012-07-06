


Console = {
	cursor = 0,
	string = '',
}
Console.__index = Console
Console.iLua = require('data/scripts/ilua')
-- Output log. Commands log in iLua console history
Console.history = {}

function Console:widget()
	if self.console == nil then
		self.console = Interface.getWidgetByName("console")
		self.lines = self.console:Lines()
	end
	return self.console
end

function Console:toggle()
	if self:widget():toggle() then
		self.old_bindings = Input.getName()
		Input.set("bindings_console")
		Input.setReciever( function(keynum, down, char)
			if down == 1 then self:process(keynum, char) end
			end
		)
	else
		Input.set( self.old_bindings )
		Input.setReciever(nil)
	end
end

function Console:process( keynum, char )
	if keynum == keys.up then
		self.string = ilua.prev_history()
	elseif keynum == keys.down then
		self.string = ilua.next_history()
	end

	if keynum == keys.enter then
		table.insert(self.history, '> ' .. self.string .. '\n')
		local process = ilua.process(self.string)
		local lines = {}
		process:gsub(
			string.format("([^%s]+)", "\n"),
			function(c) self.history[#self.history+1] = c .. '\n' end
		)
		self.string = ''
	elseif keynum == keys.backspace then
		self.string = string.sub(self.string, 1, -2)
	elseif keynum == keys.tab then
		local completion = ilua.complete(self.string)
		if type(completion) == "string" then
			self.string = completion
		elseif type(completion) == "table" then
			print(table.dump(completion))
			local maxlen = 0
			for k,v in pairs(completion) do
				if maxlen < #v then maxlen = #v end
			end
			local compl = ""
			local cols = 3
			if cols > #completion then cols = #completion end
			table.insert(self.history, '> ' .. self.string .. '\n')
			local lines = math.ceil(#completion / cols)
			cols = cols - 1
			for j = 1, lines do
				for i=0, cols do
					if completion[j + cols * i] ~= nil then
						compl = compl .. string.format("%-" .. maxlen + 1 .. "s", completion[j + cols * i])
					end
				end
				table.insert(self.history, compl .. '\n')
				compl = ""
			end
		end
	else
		self.string = self.string .. char
	end

	-- Print lines of log
	local out = ''
	local count = table.getn(self.history)
	if count > 0 then
		-- Start line
		local number = 0
		if count > self.lines then
			number = count - self.lines
		end
		for line = number, number + self.lines do
			if line < count then
				out = out .. self.history[line + 1]
			end
		end
	end
	self:widget():setText(out .. '> ' .. self.string)
end
