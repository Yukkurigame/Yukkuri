
---
local configs = {}
math.randomseed(os.time())

function load( filename )
    print("Load " .. filename)
    local file, err = io.open(filename, "r")
    --- no print on assert. wtf?
    if file == nil then
        print(err .. ' in ' .. filename)
        return
    end
    local filedata = file:read("*all")
    local data = assert(loadstring('return ' .. filedata))()
    file:close()
    ctype = string.lower(data[1])
    table.remove(data,1)
    for i,j in ipairs(data) do
        if configs[ctype] == nil then
            configs[ctype] = {}
        end
        if j.name == nil then
            print( i .. " in " .. filename ..  ": no name given. Skipped.")
            break        
        end        
        local name = string.lower(j.name)
        if configs[ctype][name] ~= nil then
            print("Config with name " .. name .. " already loaded. Skipped.\n")
            break
        end
        configs[ctype][name] = j
    end
end

function get( value, subconfig, config )
    ctype = string.lower(config)
    if configs[ctype] == nil then
        print("Config " .. ctype .. " does not exist")  
        return
    end
    csubtype = string.lower(subconfig)
    if configs[ctype][csubtype] == nil then
    	print("Subconfig " .. csubtype .. " not exists in config " .. ctype)
    	return
    end
    ret = configs[ctype][csubtype][value]
    return ret
end

function getSubconfigsList( config )
    t = {}
    for i,j in pairs(configs[string.lower(config)]) do
        table.insert(t, i)        
    end
    return t
end

function getOneFromSeveral( field, config )
    ctype = string.lower(config)
    config = configs[ctype]
    if config == nil then
        print("Config " .. ctype .. " does not exist")
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
