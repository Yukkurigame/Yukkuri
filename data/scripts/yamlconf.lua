
---
local configs = {}
local default = nil

function load( filename )
    if package.loaded.yaml == nil then require 'yaml' end
    print("Load " .. filename)
    local file, err = io.open(filename, "r")
    --- no print on assert. wtf?
    if file == nil then
        print(err .. ' in ' .. filename)
        return
    end
    local filedata = file:read("*all")    
    local yamldata = yaml.load(filedata)
    file:close()
    for i in ipairs(yamldata) do
        if yamldata[i].name == nil then break end
        local name = string.lower(yamldata[i].name)
        if configs[name] ~= nil then
            print("Config with name " .. name .. " already loaded. Skipped.\n")
            break
        end
        if default == nil then
            default = name
        end
        configs[string.lower(name)] = yamldata[i]
    end    
end

function get( value, config )
    if config == nil then
        config = default
    end
    ret = configs[string.lower(config)][value]        
    return ret
end

function getOneFromSeveral( field, config )
    local t = {}
    for i, j in pairs(configs) do
        t[i] = configs[i][field]
    end
    local min, max = 0.0, 0.0
    rnd = math.random()
    for i,j in pairs(t) do
        local el =  j / 100.0
        max = max + el
        if ( min < rnd and rnd <= max ) then
            return "Reimu"
        end
    end
end


function setDefault( value )    
    if configs[string.lower(value)] ~= nil then    
        default = string.lower(value)
    end
end

