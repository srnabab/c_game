import bpy
import os

# 设置导出格式和路径 (根据需要修改)
export_format = "FBX"  # 可选: "FBX", "OBJ", "GLTF2" 等
output_dir = bpy.path.abspath("//exported_models/")  # "//" 表示 Blender 文件所在目录

# 确保输出目录存在，如果不存在则创建
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

# 获取所有选中的对象
selected_objects = bpy.context.selected_objects

# 遍历选中的对象
for obj in selected_objects:
    if obj.type == 'MESH': # 确保只导出网格模型 (可以根据需要修改对象类型)

        # 构建导出文件名，使用对象名称
        filename = obj.name
        filepath = os.path.join(output_dir, filename + "." + export_format.lower())

        # 取消选择所有对象，然后只选择当前对象
        bpy.ops.object.select_all(action='DESELECT')
        obj.select_set(True)
        bpy.context.view_layer.objects.active = obj # 设置为活动对象 (某些导出器可能需要)

        # 执行导出操作，根据 export_format 选择不同的导出函数
        if export_format == "FBX":
            bpy.ops.export_scene.fbx(
                filepath=filepath,
                use_selection=True,  # 仅导出选中的对象
                global_scale=1.0,    # 全局缩放
                apply_unit_scale=True, # 应用单位缩放
                use_space_transform=True, # 应用空间变换
                object_types={'MESH'}, # 只导出网格
                bake_space_transform=False # 不烘焙空间变换 (如果需要可以改为 True)
                # ... 其他 FBX 导出选项，可以根据需要添加 ...
            )
        elif export_format == "OBJ":
            bpy.ops.export_scene.obj(
                filepath=filepath,
                use_selection=True,
                global_scale=1.0,
                path_mode='AUTO', # 路径模式
                use_mesh_modifiers=True, # 应用修改器
                use_edges=True, # 导出边
                use_smooth_groups=False, # 不使用平滑组 (根据需要修改)
                use_triangles=False, # 不三角化 (根据需要修改)
                keep_vertex_order=False # 不保持顶点顺序 (根据需要修改)
                # ... 其他 OBJ 导出选项，可以根据需要添加 ...
            )
        elif export_format == "GLTF2":
            bpy.ops.export_scene.gltf(
                filepath=filepath,
                use_selection=True,
                export_format='GLTF_SEPARATE', # 分离的 glTF (gltf + bin + textures)
                export_copyright="",
                export_image_format='AUTO',
                export_texture_dir="",
                export_keep_originals=False,
                export_texcoords=True,
                export_normals=True,
                export_draco_compression_enable=False,
                export_tangents=False,
                export_materials='EXPORT', # 导出材质
                export_colors=True,
                export_cameras=False,
                export_lights=False,
                export_animations=False,
                export_morph_normals=False,
                export_morph_tangents=False,
                export_export_skins=False,
                export_export_extras=False,
                export_yup=True, # Y 轴向上
                export_apply_modifiers=True, # 应用修改器
                export_animations_mode='ACTIONS', # 动画模式
                export_frame_range='CURRENT_FRAME', # 帧范围
                export_force_sampling=True, # 强制采样
                export_optimize_animation_size=False, # 优化动画大小
                export_bake_animation=False, # 不烘焙动画 (如果需要可以改为 True)
                export_limit_extension='NONE', # 限制扩展
                # ... 其他 glTF 导出选项，可以根据需要添加 ...
            )
        else:
            print(f"不支持的导出格式: {export_format}")

        print(f"已导出模型: {obj.name} 到 {filepath}")

print("所有选中的模型导出完成！")