require("data/scripts/adddebug")
require("data/scripts/bindings")
---
Configs = {}
Configs.__index = Configs

function Configs.new()
	c = {}
	c.configs = {}
	c.configs_id = {}
	setmetatable(c, Configs)
	return c
end

function Configs:load( filename )
	Debug.print(Debug.CONFIG, "Load " .. filename)
	local file, err = io.open(filename, "r")
	--- no print on assert. wtf?
	if file == nil then
		Debug.print(Debug.CONFIG, err .. ' in ' .. filename)
		return false
	end
	local filedata = file:read("*all")
	local data = assert(loadstring('return ' .. filedata))()
	file:close()
	local records = 0
	local success = 0
	for i,j in ipairs(data) do
		records = records + 1
		repeat
			if j.type == nil then
				Debug.print(Debug.CONFIG, "Config has no type.")
				break
			end
			ctype = string.lower(j.type)
			if self.configs[ctype] == nil then
				self.configs[ctype] = {}
			end
			if j.id == nil then
				Debug.print(Debug.CONFIG, "Error in " .. filename ..  ": no id given. Skipped.")
				break
			end
			local id = string.lower(j.id)
			if self.configs[ctype][id] ~= nil then
				Debug.print(Debug.CONFIG, "Config with id " .. id .. " already loaded. Skipped.")
				break
			end
			self.configs[ctype][id] = j
			if self.configs_id[id] ~= nil then
				Debug.print(Debug.CONFIG, "Config with id " .. id .. " already exists. Collision occured.")
			end
			self.configs_id[id] = j
			success = success + 1
		until true
	end
	if success ~= records then
		Debug.print(Debug.CONFIG, filename .. ": loaded " .. success .. " from " .. records .. " records.")
	end
	return true
end

function Configs:loadAll( type )
	path = self:get("configs_path", "config_general", "config")
	if path == nil then
		path = getcwd() .. "/" .. "data/defs/"
	end
	listing = readdir(path)
	if listing == nil then
		Debug.print(Debug.CONFIG, "Bad directory " .. path)
		return
	end
	files = 0
	success = 0
	for i, filename in ipairs(listing) do
		if filename:match("%a+%." .. type .. "$") then
			files = files + 1
			if self:load(path .. filename) then
				success = success + 1
			end
		end
	end
	Debug.print(Debug.CONFIG, "Loaded " .. success .. " from " .. files .. " " .. type .. " files.")
end

function Configs:getSubconfig( subconfig, config )
	ctype = string.lower(config)
	if self.configs[ctype] == nil then
		Debug.print(Debug.CONFIG, "Config " .. ctype .. " does not exist.")
		return
	end
	csubtype = string.lower(subconfig)
	if self.configs[ctype][csubtype] == nil then
		Debug.print(Debug.CONFIG, "Subconfig " .. csubtype .. " not exists in config " .. ctype)
		return
	end
	return self.configs[ctype][csubtype]
end


function Configs:get( value, subconfig, config )
	local sconfig = self:getSubconfig(subconfig, config)
	if sconfig == nil then return end
--[[
	ctype = string.lower(config)
	if self.configs[ctype] == nil then
		Debug.print(Debug.CONFIG, "Config " .. ctype .. " does not exist.")
		return
	end
	csubtype = string.lower(subconfig)
	if self.configs[ctype][csubtype] == nil then
		Debug.print(Debug.CONFIG, "Subconfig " .. csubtype .. " not exists in config " .. ctype)
		return
	end
	ret = self.configs[ctype][csubtype][value]
-- ]]--
	ret = sconfig[value]
	return ret
end

function Configs:getById( value, id )
	ret = self.configs_id[id]
	if ret == nil then
		Debug.print(Debug.CONFIG, "Config with id " .. id .. " does not exists.")
	else
		ret = ret[value]
	end
	return ret
end

function Configs:getSubconfigs( config )
	t = {}
	cname = string.lower(config)
	if self.configs[cname] == nil then
		Debug.print(Debug.CONFIG, "Config " .. cname .. " does not exist.")
	else
		for i,j in pairs(self.configs[cname]) do
			table.insert(t, j)
		end
	end
	return t
end

function Configs:getSubconfigsList( config )
	t = {}
	cname = string.lower(config)
	if self.configs[cname] == nil then
		Debug.print(Debug.CONFIG, "Config " .. cname .. " does not exist.")
	else
		for i,j in pairs(self.configs[cname]) do
			table.insert(t, i)
		end
	end
	return t
end

function Configs:getSubconfigsLength( config )
	len = 0
	cname = string.lower(config)
	if self.configs[cname] == nil then
		Debug.print(Debug.CONFIG, "Config " .. cname .. " does not exist.")
	else
		--- Я не смог в #
		for i in pairs(self.configs[cname]) do
			len = len + 1
		end
	end
	return len
end

function Configs:getOneFromSeveral( field, config )
	local ctype = string.lower(config)
	local config = self.configs[ctype]
	if config == nil then
		Debug.print(Debug.CONFIG, "Config " .. ctype .. " does not exist.")
		return
	end

	local t = {}
	for i, j in pairs(config) do
		t[i] = config[i][field]
	end
	local min, max = 0.0, 0.0
	rnd = math.random()
	for i,j in pairs(t) do
		local el =  j / 100.0
		max = max + el
		if ( min < rnd and rnd <= max ) then
			return i
		end
	end
end

return Configs
