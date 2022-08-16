package.path = package.path .. ";../../app/?.lua;../../app/demo/?.lua"

_G.log_tree = require "tree"
local navmesh = require "navmesh"

local function init_navmesh()
    local query_extent = {x=50, y=50, z=250}
    local include_flag = 32767
    local exclude_flag = 0
    local navmesh_instance = navmesh.create("../../app/demo/MainCity_Main_Mannequin.navmesh", include_flag, exclude_flag, query_extent)
    assert(navmesh_instance)
    log_tree("navmesh", navmesh)
    log_tree("navmesh_instance", navmesh_instance)
    log_tree("navmesh_instance_meta", getmetatable(navmesh_instance))
    -- navmesh_instance.set_flags(32767, 0);
    navmesh_instance.set_flags_(32767, 0);
    -- local path_result = navmesh_instance.find_path({x= 620.40216064453, y = -100, z = 714.91369628906}, {x= 620.40216064453, y = 330, z = 714.91369628906})
    -- log_tree("path_result", path_result)
end

function init()
    init_navmesh()
    log_tree("navmesh", navmesh)
end

init()

local student = require "student"

local first = student.new()
log_tree("xxstudent", student)
log_tree("xxfirst", first)
log_tree("xxfirst", getmetatable(first))
first.setName("Nioh1")
local strName = first:getName()
log_tree("strName", strName)
log_tree("first", getmetatable(first))

-- local second = student.new()

-- second:setName("Nioh2")
-- local strName = second:getName()
-- log_tree("strName", strName)
-- log_tree("second", getmetatable(second))

-- first:setName("Nioh3")
-- local strName = first:getName()
-- log_tree("strName", strName)
-- log_tree("second", second:getName())