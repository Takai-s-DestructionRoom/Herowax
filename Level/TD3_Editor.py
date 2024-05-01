import bpy
import math
import bpy_extras
import gpu
import gpu_extras.batch
import copy
import mathutils
import json
import os

# ブレンダーに登録するアドオン情報
bl_info = {
	"name": "レベルエディタ",
	"author": "Takai Rikushi",
	"version": (1,0),
	"blender": (4,0,0),
	"location": "",
	"description": "レベルエディタ",
	"wiki_uri": "",
	"category": "object"
}

#コライダー描画
class DrawCollider:
	#描画ハンドル
	handle = None

	#3Dビューに登録する描画関数
	def draw_collider():
		#頂点データ
		vertices = {"pos":[]}
		#インデックスデータ
		indices = []

		offsets = [
			[-0.5,-0.5,-0.5],
			[+0.5,-0.5,-0.5],
			[-0.5,+0.5,-0.5],
			[+0.5,+0.5,-0.5],
			[-0.5,-0.5,+0.5],
			[+0.5,-0.5,+0.5],
			[-0.5,+0.5,+0.5],
			[+0.5,+0.5,+0.5],
		]

		size = [2,2,2]

		#現在のシーンのオブジェクトの数だけ回す
		for object in bpy.context.scene.objects:
			#コライダープロパティがなければ、描画をスキップ
			if not "collider" in object:
				continue

			#変数宣言(中身の意味はなさそう)
			center = mathutils.Vector((0,0,0))
			size = mathutils.Vector((2,2,2))

			#プロパティから値を取得
			center[0]=object["collider_center"][0]
			center[1]=object["collider_center"][1]
			center[2]=object["collider_center"][2]
			size[0]=object["collider_size"][0]
			size[1]=object["collider_size"][1]
			size[2]=object["collider_size"][2]

			start = len(vertices["pos"])

			#Boxの8頂点分回す
			for offset in offsets:
				pos = copy.copy(center)

				pos[0]+=offset[0]*size[0]
				pos[1]+=offset[1]*size[1]
				pos[2]+=offset[2]*size[2]

				pos = object.matrix_world @ pos

				vertices['pos'].append(pos)

				indices.append([start + 0,start + 1])
				indices.append([start + 2,start + 3])
				indices.append([start + 0,start + 2])
				indices.append([start + 1,start + 3])

				indices.append([start + 4,start + 5])
				indices.append([start + 6,start + 7])
				indices.append([start + 4,start + 6])
				indices.append([start + 5,start + 7])

				indices.append([start + 0,start + 4])
				indices.append([start + 1,start + 5])
				indices.append([start + 2,start + 6])
				indices.append([start + 3,start + 7])
		
		#ビルドインのシェーダを取得
		shader = gpu.shader.from_builtin("3D_UNIFORM_COLOR")
		batch = gpu_extras.batch.batch_for_shader(shader,"LINES",vertices,indices = indices)
		#シェーダのパラメータ設定
		color = [0.5,1.0,1.0,1.0]
		shader.bind()
		shader.uniform_float("color",color)
		#描画
		batch.draw(shader)

#def draw_menu_manual(self,context):
#	self.layout.operator("wm.url_open_preset",text = "Manual",icon='HELP')

class OBJECT_PT_collider(bpy.types.Panel):
	bl_name = "OBJECT_PT_collider"
	bl_label = "collider"
	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context = "object"

	def draw(self,context):
		#サブメニューの描画
		if "collider" in context.object:
			#すでにプロパティがあれば、プロパティを表示
			self.layout.prop(context.object,'["collider"]',text="Type")
			self.layout.prop(context.object,'["collider_center"]',text="Center")
			self.layout.prop(context.object,'["collider_size"]',text="Size")
		else:
			#なければ、ボタンを表示
			self.layout.operator(MYADDON_OT_add_collider.bl_idname,
					text=MYADDON_OT_add_collider.bl_label)

#読み込み時にそこにオブジェクトを配置するタグを追加するクラス
class MYADDON_OT_add_setObject(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_add_setobject"
	bl_label = "直接配置オブジェクト 追加"
	bl_description = "['setObject']カスタムプロパティを追加します"
	bl_options = {"REGISTER","UNDO"}

	def execute(self,context):
		#['setObject']カスタムプロパティを追加
		context.object["setObject"] = ""

		return {"FINISHED"}
	
#スポナー用に入力欄を追加
class MYADDON_OT_add_spawnerOrder(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_spawnerorder"
	bl_label = "スポナーオーダー 追加"
	bl_description = "このスポナーが使用するスポナーオーダーのファイルを指定します"
	bl_options = {"REGISTER","UNDO"}

	def execute(self,context):
		#['spawnerOrder']カスタムプロパティを追加
		context.object["spawnerOrder"] = ""

		return {"FINISHED"}
	
#スポナー用に入力欄を追加
class MYADDON_OT_add_behaviorOrder(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_behaviororder"
	bl_label = "ビヘイビア 追加"
	bl_description = "この敵が使用するビヘイビア(挙動)のファイルを指定します"
	bl_options = {"REGISTER","UNDO"}

	def execute(self,context):
		#['behaviorOrder']カスタムプロパティを追加
		context.object["behaviorOrder"] = ""

		return {"FINISHED"}

#コライダーを追加するクラス
class MYADDON_OT_add_collider(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_add_collider"
	bl_label = "コライダー 追加"
	bl_description = "['collider']カスタムプロパティを追加します"
	bl_options = {"REGISTER","UNDO"}

	def execute(self,context):
		#['collider']カスタムプロパティを追加
		context.object["collider"] = "BOX"
		context.object["collider_center"] = mathutils.Vector((0,0,0))
		context.object["collider_size"] = mathutils.Vector((2,2,2))

		return {"FINISHED"}

class OBJECT_PT_setObject(bpy.types.Panel):
	"""オブジェクトのファイルネームパネル"""
	bl_idname = "OBJECT_PT_setObject"
	bl_label = "setObject"
	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context = "object"

	#サブメニューの描画
	def draw(self,context):
		if "setObject" in context.object:
			#すでにプロパティがあれば、プロパティを表示
			self.layout.prop(context.object,'["setObject"]',text=self.bl_label)
		else:
			self.layout.operator(MYADDON_OT_add_setObject.bl_idname,
					text=MYADDON_OT_add_setObject.bl_label)
			
class OBJECT_PT_spawnerOrder(bpy.types.Panel):
	"""オブジェクトのファイルネームパネル"""
	bl_idname = "OBJECT_PT_spawnerOrder"
	bl_label = "spawnerOrder"
	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context = "object"

	#サブメニューの描画
	def draw(self,context):
		if "spawnerOrder" in context.object:
			#すでにプロパティがあれば、プロパティを表示
			self.layout.prop(context.object,'["spawnerOrder"]',text=self.bl_label)

class OBJECT_PT_behaviorOrder(bpy.types.Panel):
	"""オブジェクトのファイルネームパネル"""
	bl_idname = "OBJECT_PT_behaviorOrder"
	bl_label = "behaviorOrder"
	bl_space_type = "PROPERTIES"
	bl_region_type = "WINDOW"
	bl_context = "object"

	#サブメニューの描画
	def draw(self,context):
		if "behaviorOrder" in context.object:
			#すでにプロパティがあれば、プロパティを表示
			self.layout.prop(context.object,'["behaviorOrder"]',text=self.bl_label)


class MYADDON_OT_export_scene(bpy.types.Operator,bpy_extras.io_utils.ExportHelper):
	bl_idname = "myaddon.myaddon_ot_export_scene"
	bl_label = "シーン出力"
	bl_description = "シーン情報をExportします"
	#出力するファイルの拡張子
	filename_ext = ".json"

	def execute(self,context):

		print("シーン情報をExportします")

		self.export_json()

		print("シーン情報をExportしました")
		self.report({'INFO'},"シーン情報をExportしました")

		return {'FINISHED'}
				
	def export_json(self):
		"""JSON形式でファイルに出力"""

		#保存する情報をまとめるdict
		json_object_root = dict()

		#ノード名
		json_object_root["name"] = "scene"
		#オブジェクトリストを作成
		json_object_root["objects"] = list()
		
		#シーン内の全オブジェクトを走査してパック
		for object in bpy.context.scene.objects:
			#子供ならスキップ(代わりに親から呼び出すから)
			if(object.parent):
				continue

			#シーン直下のオブジェクトをルートノード(深さ0)とし、再起関数で走査
			self.parse_scene_recursive_json(json_object_root["objects"],object,0)

		#オブジェクトをJSON文字列にエンコード
		json_text = json.dumps(json_object_root,ensure_ascii=False,cls=json.JSONEncoder,indent=4)

		print(json_text)

		#ファイルをテキスト形式で書き出し用にオープン
		#スコープを抜けると自動的にクローズされる
		with open(self.filepath,"wt",encoding="utf-8")as file:

			#ファイルに文字列を書き込む
			file.write(json_text)

	def write_and_print(self,file,str):
		print(str)

		file.write(str)
		file.write('\n')

	def parse_scene_recursive_json(self,data_parent,object,level):
		#シーンのオブジェクト1個分のjsonオブジェクト生成
		json_object = dict()
		#オブジェクト種類
		json_object["type"] = object.type
		#オブジェクト名
		json_object["name"] = object.name

		#Todo:その他情報をパック
		#オブジェクトのローカルトランスフォームから
		#平行移動、回転、スケールを抽出
		#trans,rot,scale = object.matrix_local.decompose()
		trans,rot,scale = object.matrix_world.decompose()
		#回転をQuternionからEuler(3軸での回転角)に変換
		rot = rot.to_euler()
		#ラジアンから度数法に変換
		rot.x = math.degrees(rot.x)
		rot.y = math.degrees(rot.y)
		rot.z = math.degrees(rot.z)
		#トランスフォーム情報をディクショナリに登録
		transform = dict()
		transform["translation"] = (trans.x,trans.y,trans.z)
		transform["rotation"] = (rot.x,rot.y,rot.z)
		transform["scaling"] = (scale.x,scale.y,scale.z)
		#まとめて1個分のjsonオブジェクトに登録
		json_object["transform"] = transform

		#カスタムプロパティ'collider'
		if "collider" in object:
			collider = dict()
			collider["type"] = object["collider"]
			collider["center"] = object["collider_center"].to_list()
			collider["size"] = object["collider_size"].to_list()
			json_object["collider"]=collider

		if "setObject" in object:
			json_object["setObject"] = object["setObject"]

		if "spawnerOrder" in object:
			json_object["spawnerOrder"] = object["spawnerOrder"]

		if "behaviorOrder" in object:
			json_object["behaviorOrder"] = object["behaviorOrder"]

		

		#1個分のjsonオブジェクトを親オブジェクトに登録
		data_parent.append(json_object)

		#Todo:直接の子供リストを走査
		#子ノードがあれば
		if len(object.children) > 0:
			#子ノードリストを作成
			json_object["children"] = list()

			#子ノードへ進む(深さが1上がる)
			for child in object.children:
				self.parse_scene_recursive_json(json_object["children"],child,level + 1)

#敵のスポナーの配置
class MYADDON_OT_EnemySpawner(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_enemyspawner"
	bl_label = "敵のスポナーを作成"
	bl_description = "敵のスポナーを3Dカーソルの位置に作成します。"
	bl_options = {'REGISTER','UNDO'}

	#メニューを実行した時に呼ばれる関数
	def execute(self,context):
		path = bpy.data.filepath
		
		print("path = " + path)

		dirpath = os.path.dirname(path)
		objpath = dirpath + "\Resources\stick\stick.obj"
		print("objpath = " + objpath)
		bpy.ops.wm.obj_import(filepath=objpath)
		import_obj = bpy.context.selected_objects[0]
		import_obj['setObject'] = 'EnemySpawner'
		
		#軸変換による余計な回転が入っているので適用して解除
		bpy.ops.object.transform_apply(rotation=True)

		import_obj.name = "敵スポナー"

		import_obj.location = context.scene.cursor.location
		import_obj.scale = mathutils.Vector((1,1,1))
		import_obj.rotation_euler = mathutils.Vector((0,1.57,0))

		import_obj['spawnerOrder'] = ""

		import_obj['behaviorOrder'] = ""

		#オペレータの命令終了を通知
		return {'FINISHED'}
	
#敵の配置
class MYADDON_OT_SetEnemy(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_setenemy"
	bl_label = "敵を配置"
	bl_description = "敵を3Dカーソルの位置に作成します。"
	bl_options = {'REGISTER','UNDO'}

	#メニューを実行した時に呼ばれる関数
	def execute(self,context):
		path = bpy.data.filepath
		
		print("path = " + path)

		dirpath = os.path.dirname(path)
		objpath = dirpath + "\Resources\enemy\enemy.obj"
		print("objpath = " + objpath)
		bpy.ops.wm.obj_import(filepath=objpath)
		import_obj = bpy.context.selected_objects[0]
		import_obj['setObject'] = 'Enemy'
		
		#軸変換による余計な回転が入っているので適用して解除
		bpy.ops.object.transform_apply(rotation=True)

		import_obj.name = "敵"

		import_obj.location = context.scene.cursor.location
		import_obj.scale = mathutils.Vector((1,1,1))
		import_obj.rotation_euler = mathutils.Vector((0,0,0))

		import_obj['behaviorOrder'] = ""

		#オペレータの命令終了を通知
		return {'FINISHED'}
	
#鳥の巣(守る対象)の配置
class MYADDON_OT_Birdnest(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_birdnest"
	bl_label = "鳥の巣(守る対象)を作成"
	bl_description = "鳥の巣(守る対象)を3Dカーソルの位置に作成します。"
	bl_options = {'REGISTER','UNDO'}

	#メニューを実行した時に呼ばれる関数
	def execute(self,context):
		path = bpy.data.filepath
		
		print("path = " + path)

		dirpath = os.path.dirname(path)
		objpath = dirpath + "\Resources\Birdnest\Birdnest.obj"
		print("objpath = " + objpath)
		bpy.ops.wm.obj_import(filepath=objpath)
		import_obj = bpy.context.selected_objects[0]
		import_obj['setObject'] = 'Tower'
		
		#軸変換による余計な回転が入っているので適用して解除
		bpy.ops.object.transform_apply(rotation=True)

		import_obj.name = "鳥の巣"

		import_obj.location = context.scene.cursor.location
		import_obj.scale = mathutils.Vector((1,1,1))

		#オペレータの命令終了を通知
		return {'FINISHED'}
	
#地面の配置
class MYADDON_OT_Ground(bpy.types.Operator):
	bl_idname = "myaddon.myaddon_ot_ground"
	bl_label = "地面を作成"
	bl_description = "地面を3Dカーソルの位置に作成します。"
	bl_options = {'REGISTER','UNDO'}

	#メニューを実行した時に呼ばれる関数
	def execute(self,context):
		path = bpy.data.filepath
		
		print("path = " + path)

		dirpath = os.path.dirname(path)
		objpath = dirpath + "\Resources\Ground\ground.obj"
		print("objpath = " + objpath)
		bpy.ops.wm.obj_import(filepath=objpath)
		import_obj = bpy.context.selected_objects[0]
		import_obj['setObject'] = 'Ground'
		
		#軸変換による余計な回転が入っているので適用して解除
		bpy.ops.object.transform_apply(rotation=True)

		import_obj.name = "地面"

		import_obj.location = context.scene.cursor.location
		import_obj.scale = mathutils.Vector((1,1,1))

		#オペレータの命令終了を通知
		return {'FINISHED'}

#トップバーの拡張メニュー
class TOPBAR_MT_editor_menu(bpy.types.Menu):
	bl_idname = "TOPBAR_MT_editor_menu"

	bl_label = "エディタメニュー"

	bl_description = "拡張メニュー by " + bl_info["author"]

	def draw(self,context):
		self.layout.operator(MYADDON_OT_export_scene.bl_idname,
		       text=MYADDON_OT_export_scene.bl_label)
		self.layout.separator()
		self.layout.operator(MYADDON_OT_EnemySpawner.bl_idname,
		       text=MYADDON_OT_EnemySpawner.bl_label)
		self.layout.operator(MYADDON_OT_Ground.bl_idname,
		       text=MYADDON_OT_Ground.bl_label)
		self.layout.operator(MYADDON_OT_SetEnemy.bl_idname,
		       text=MYADDON_OT_SetEnemy.bl_label)

	def submenu(self,context):
		self.layout.menu(TOPBAR_MT_editor_menu.bl_idname)

classes = (
	MYADDON_OT_export_scene,
	TOPBAR_MT_editor_menu,
	MYADDON_OT_add_collider,
	OBJECT_PT_collider,
	MYADDON_OT_add_setObject,
	OBJECT_PT_setObject,
	MYADDON_OT_add_spawnerOrder,
	OBJECT_PT_behaviorOrder,
	MYADDON_OT_add_behaviorOrder,
	OBJECT_PT_spawnerOrder,
	MYADDON_OT_EnemySpawner,
	MYADDON_OT_Birdnest,
	MYADDON_OT_Ground,
	MYADDON_OT_SetEnemy,
)

def register():
	#チェックボックス追加用のなんか
	bpy.types.Scene.my_checkbox_property = bpy.props.BoolProperty(name="my_checkbox_property")

	#Blenderにクラスを登録
	for cls in classes:
		bpy.utils.register_class(cls)

	#メニューに項目を追加
	bpy.types.TOPBAR_MT_editor_menus.append(TOPBAR_MT_editor_menu.submenu)
	#3Dビューに描画関数を追加
	DrawCollider.handle = bpy.types.SpaceView3D.draw_handler_add(DrawCollider.draw_collider,(),"WINDOW","POST_VIEW")
	
	print("レベルエディタが有効化されました。")

def unregister():
	#メニューから項目を削除
	bpy.types.TOPBAR_MT_editor_menus.remove(TOPBAR_MT_editor_menu.submenu)
	#3Dビューから描画関数を削除
	bpy.types.SpaceView3D.draw_handler_remove(DrawCollider.handle,"WINDOW")

	print("レベルエディタが無効化されました。")
	#Blenderからクラスを削除
	for cls in classes:
		bpy.utils.unregister_class(cls)