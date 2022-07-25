package.path = package.path .. ";../../app/?.lua;../../app/demo/?.lua"

_G.log_tree = require "tree"
local navmesh = require "navmesh"

local function init_navmesh()
    local query_extent = {x=50, y=50, z=250}
    local include_flag = 32767
    local exclude_flag = 0
    local navmesh_instance = navmesh.create("../../app/demo/MainCity_Main_Mannequin.navmesh", include_flag, exclude_flag, query_extent)
    assert(navmesh_instance)

    local path_result = navmesh_instance.find_path({x= 620.40216064453, y = -100, z = 714.91369628906}, {x= 620.40216064453, y = 330, z = 714.91369628906})
    log_tree("path_result", path_result)
end

function init()
    init_navmesh()
end

init()