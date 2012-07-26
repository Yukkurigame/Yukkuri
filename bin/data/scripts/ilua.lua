-- ilua.lua
-- A more friendly Lua interactive prompt
-- doesn't need '='
-- will try to print out tables recursively, subject to the pretty_print_limit value.
-- Steve Donovan, 2007
--
local pretty_print_limit = 20
local max_depth = 7
local table_clever = true
local prompt = '> '
local verbose = false
local strict = true
-- suppress strict warnings
_ = true

-- imported global functions
local sub = string.sub
local match = string.match
local find = string.find
local push = table.insert
local pop = table.remove
local append = table.insert
local concat = table.concat
local floor = math.floor

local collisions = {}
local G_LIB = {}
local declared = {}
local line_handler_fn, global_handler_fn
local print_handlers = {}

ilua = {}
local num_prec
local num_all

local jstack = {}

local output = ''

local readline = pcall(require, "data/scripts/3rdparty/rlcompleter")

local function oprint(...)
	for i, v in ipairs(arg) do
		output = output .. tostring(v)
	end
	output = output .. '\n'
end

local function join(tbl, delim, limit, depth)
	if not limit then limit = pretty_print_limit end
	if not depth then depth = max_depth end
	local n = #tbl
	local res = ''
	local k = 0
	-- very important to avoid disgracing ourselves with circular referencs...
	if #jstack > depth then
		return "..."
	end
	for i,t in ipairs(jstack) do
		if tbl == t then
			return "<self>"
		end
	end
	push(jstack,tbl)
	-- this is a hack to work out if a table is 'list-like' or 'map-like'
	-- you can switch it off with ilua.table_options {clever = false}
	local is_list
	if table_clever then
		local index1 = n > 0 and tbl[1]
		local index2 = n > 1 and tbl[2]
		is_list = index1 and index2
	end
	if is_list then
		for i,v in ipairs(tbl) do
			res = res..delim..val2str(v)
			k = k + 1
			if k > limit then
				res = res.." ... "
				break
			end
		end
	else
		for key,v in pairs(tbl) do
			if type(key) == 'number' then
				key = '['..tostring(key)..']'
			else
				key = tostring(key)
			end
			res = res..delim..key..'='..val2str(v)
			k = k + 1
			if k > limit then
				res = res.." ... "
				break
			end
		end
	end
	pop(jstack)
	return sub(res,2)
end


local function val2str(val)
	local tp = type(val)
	if print_handlers[tp] then
		local s = print_handlers[tp](val)
		return s or '?'
	end
	if tp == 'function' then
		return tostring(val)
	elseif tp == 'table' then
		if val.__tostring  then
			return tostring(val)
		else
			return '{'..join(val,',')..'}'
		end
	elseif tp == 'string' then
		return "'"..val.."'"
	elseif tp == 'number' then
		-- we try only to apply floating-point precision for numbers deemed to be floating-point,
		-- unless the 3rd arg to precision() is true.
		if num_prec and (num_all or floor(val) ~= val) then
			return num_prec:format(val)
		else
			return tostring(val)
		end
	else
		return tostring(val)
	end
end

function ilua._pretty_print(...)
	for i,val in ipairs(arg) do
		oprint(val2str(val))
	end
	_G['_'] = arg[1]
end

local function compile(line)
	if verbose then oprint(line) end
	local f,err = loadstring(line,'local')
	return err,f
end

local function evaluate(chunk)
	local ok,res = pcall(chunk)
	if not ok then
		return res
	end
	return nil -- meaning, fine!
end

local function eval_lua(line)
	-- is the line handler interested?
	if line_handler_fn then
		line = line_handler_fn(line)
		-- returning nil here means that the handler doesn't want
		-- Lua to see the string
		if not line then return end
	end
	-- is it an expression?
	local err,chunk = compile('ilua._pretty_print('..line..')')
	if err then
		-- otherwise, a statement?
		err,chunk = compile(line)
	end
	-- if compiled ok, then evaluate the chunk
	if not err then
		err = evaluate(chunk)
	end
	-- if there was any error, print it out
	if err then
		oprint(err)
	end
end

-- functions available in scripts
function ilua.precision(len, prec, all)
	if not len then num_prec = nil
	else
		num_prec = '%'..len..'.'..prec..'f'
	end
	num_all = all
end

function ilua.table_options(t)
	if t.limit then pretty_print_limit = t.limit end
	if t.depth then max_depth = t.depth end
	if t.clever ~= nil then table_clever = t.clever end
end

-- inject @tbl into the global namespace
function ilua.import(tbl, dont_complain, lib)
	lib = lib or '<unknown>'
	if type(tbl) == 'table' then
		for k,v in pairs(tbl) do
			local key = rawget(_G,k)
			-- NB to keep track of collisions!
			if key and k ~= '_M' and k ~= '_NAME' and k ~= '_PACKAGE' and k ~= '_VERSION' then
				append(collisions,{k,lib,G_LIB[k]})
			end
			_G[k] = v
			G_LIB[k] = lib
		end
	end
	if not dont_complain and  #collisions > 0  then
		for i, coll in ipairs(collisions) do
			local name,lib,oldlib = coll[1],coll[2],coll[3]
			oprint('warning: ',lib,'.',name,' overwrites ')
			if oldlib then
				oprint(oldlib,'.',name,'\n')
			else
				oprint('global ',name,'\n')
			end
		end
	end
end

function ilua.print_handler(name, handler)
	print_handlers[name] = handler
end

function ilua.line_handler(handler)
	line_handler_fn = handler
end

function ilua.global_handler(handler)
	global_handler_fn = handler
end

function ilua.print_variables()
	for name,v in pairs(declared) do
		oprint(name,type(_G[name]))
	end
end
--
-- strict.lua
-- checks uses of undeclared global variables
-- All global variables must be 'declared' through a regular assignment
-- (even assigning nil will do) in a main chunk before being used
-- anywhere.
--
local function set_strict()
	local mt = getmetatable(_G)
	if mt == nil then
		mt = {}
		setmetatable(_G, mt)
	end

	local function what ()
		local d = debug.getinfo(3, "S")
		return d and d.what or "C"
	end

	mt.__newindex = function (t, n, v)
		declared[n] = true
		rawset(t, n, v)
	end

	mt.__index = function (t, n)
		if not declared[n] and what() ~= "C" then
			local lookup = global_handler_fn and global_handler_fn(n)
			if not lookup then
				error("variable '"..n.."' is not declared", 2)
			else
				return lookup
			end
		end
		return rawget(t, n)
	end

end


function ilua:start()
	-- any import complaints?
	self:import()
	-- set input to stream
	if not readline then
		Debug(2, "No readline module. No interactive console.")
	else
		rlcompleter.set_input_source(1)
	end
end

function ilua.process(line)
	output = ''
	ilua.add_history(line)
	eval_lua(line)
	return output
end

function ilua.complete(line)
	if readline then
		local rl = rlcompleter.complete(line)
		if rl ~= nil then
			return rl
		end
	end
	return ""
end

function ilua.prev_history()
	if readline then
		local rl = rlcompleter.previous_history()
		if rl ~= nil then
			return rl
		end
	end
	return ""
end

function ilua.next_history()
	if readline then
		local rl = rlcompleter.next_history()
		if rl ~= nil then
			return rl
		end
	end
	return ""
end

function ilua.add_history(line)
	if readline then
		rlcompleter.add_history(line)
		rlcompleter.next_history()
	end
end


ilua:start()

