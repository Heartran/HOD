--
-- Opening
--

-- �^�C�g����ʎ���
subclass("Opening", Actor) {
		__tostring = function(self)
			return("class Title name="..self.name) 
		end
}

-- �R���X�g���N�^
function Opening:ctor(...)
	print("Opening:ctor() called")
	Opening.super.ctor(self, ...)
end

-- ������
function Opening:init(cfg)
	-- �X�[�p�[�N���X(Actor)��init���Ă�
	print("Opening:init");
	Actor.init(self, cfg)
end

-- Actor�폜���Ɏ��s�����
function Opening:on_destroy()
	print("Opening:on_destroy()");
end


-- ��Ԋ֐�

function Opening:state_start(rt)
	print("Opening:state_start");
	
	-- FixedCamera�̐ݒ�
	local pos = D3DXVECTOR3:new_local( 200, 0, 0 )
	local lookAt = D3DXVECTOR3:new_local( 0, 0, 0 )
	local up = D3DXVECTOR3:new_local( 0, 1, 0 )
	self.camera = Camera.FixedCamera:new_local(
		pos,
		lookAt,
		up,
		45,-- fieldOFViewwInDegree
		0.1, --near
		30000) --far
		
	
	-- Model�̃��[�f�B���O
	-- �~�N
	local xmlMiku = XmlLoader.XmlLoader.MME.Object.ModelKai:new_local()
	xmlMiku:loadStart("")
	while not xmlMiku:isReady() do
		��������l������
		state:displayString("loading miku")
		self:wait(0)
	end
	self.miku = Lua.ModelRootTransformTimeLineControllerModel(
		xmlMiku:instance() )
	
	player���ǂ���邩
	ModelTimeLineController�؂�ւ����ǂ�����
	��{�l���Ă������Ƃ̓J�b�g�V�[���ƃQ�[���̐؂�ւ�
	�V�������ꍇ
	�؂�ւ���ꍇ
	
	
	�܂���player�쐬
	�V�K�Q�[���̏ꍇ
	�J�����̎d�l�����܂��Ă��Ȃ��̂�C++����Œ�J����
	�J�����̕ύX�݂����Ȃ��Ƃ��K�v����
	
	
	
	���[�h�Q�[���̏ꍇ
	�Z�[�u�̎d�l�����߂Ȃ��Ƃ����Ȃ�
	���Ԃ͌�ŏo��������
	�܂��l���Ȃ����������͂Ȃ�ƂȂ����ɂ��邩�炢��
	<Miku>
		<Status/>
		<SelectAble/>
	</Miku>
	
	
	--[[
	lua�΂��tittle�~����
	if(newgame)
		opening�ɐi��
	]]
	
	
	
	local xmlCharacter = XmlLoader.GameObject.Utils.Character:new()
	xmlCharacter:loadStart("Data/XmlLoader/GameObject/Utils/Character/miku.xml")
	while not xmlCharacter:isReady do
		byouga
	end
	
	local pCharacter = xmlCharacter:instance()
	
	-- �����X�e�[�^�X�̓ǂݍ��݂��������ŏ������ق��������̂ł�
	local pStatus = GameObjectData.Status.create()
	local status = pStatus:get()
	status:str   = 100
	status:inte  = 100
	status:will  = 100
	status:dex   = 100
	status:qui   = 100
	status:vit   = 100
	status:psy   = 100
	status:luck  = 100
	
	status:maxHp = 10000
	status:maxMp = 100
	status:maxLp = 10
	status:Stamina = 100
	status:elementalDefs = 0
	status:mass  = 30
	status:speed = 50
	status:speedLow = 25
	status:speedHigh = 75
	status:angleSpeed = 0.78500003
	status:accel = 14
	status:usageStaminaPerSecSpeedHigh = 30
	status:usageStaminaPerSecSpeed  = 1
	status:usageStaminaPerSecSpeedLow = 1
	status:usageStaminaPerJump = 10
	status:healingStaminaPerSec = 15
	status:group = 0
	
	self.pPlayer = GameObject::create( pCharacter, pStatus )
	

	local model =Lua.TimeLineCotrollerModel( pCharacter )
	model.insert(
		0,--time
		0, 0, 0, --pos
		0, 0, 0, --rotate
		"stand" )
		
	model.

	
	
	-- �_�l��
	local xmlBou = XmlLoader.XmlLoader.MME.Object.ModelKai:new_local()
	xmlBou:loadStart("")
	while not xmlBou:isReady() do
		��������l������
		state:displayString("loading �_�l��")
		self:wait(0)
	end
	

	-- �����̕���
	
	self.pStage = GameObject::Stage::create("Data/XmlLoader/GameObject/Stage/stage02.xml");
	
	-- MME�̃��[�f�B���O
	local gameDrawer = MME.Drawer.GameDrawer:instance();
	while not gameDrawer:isReady() do
		��������l������
		state:displayString("loading MME")
		self:wait(0)
	end
	

	
	self.time = 0
	
	local state = Sequence.HatuneOfTheDead.LuaImpl.State:instance()
	
	state:setCameraAnimation(self.camera);
	-- state:setEnemy(mEnemy);
	state:setPlayer(self.player);
	state:setStage(self.stage);
	
	return "goto", "state_frame"
	
end

function Opening:state_frame(rt)
	local gameDrawer = MME.Drawer.GameDrawer:instance();
	local mme = MME.Manager:instance()
	local player = self.player:get()
	local fw = GameLib.Framework:instance();

	--�Œ�t���[�����[�g
	local elapsedTime=1.0/60.0;

	self.time = self.time+elapsedTime
	
	local black = GameLib.Math.Vector4:new_local(1.0)
	mme:setEdgeColor( black );

	
	local state = Sequence.HatuneOfTheDead.LuaImpl.State:instance()
	
	while true do
		state:update()
		if true then
			change_scene(Stage01)
		end
		
		state:draw()
		self:wait(0)
	end
	

	return "exit"
end

