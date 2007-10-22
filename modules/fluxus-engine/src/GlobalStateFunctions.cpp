// Copyright (C) 2007 Dave Griffiths
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <assert.h>
#include "SchemeHelper.h"
#include "Engine.h"
#include "GlobalStateFunctions.h"
#include "Renderer.h"
 
using namespace GlobalStateFunctions;
using namespace SchemeHelper;
using namespace Fluxus;

// StartSectionDoc-en
// global-state
// Global state is really anything that controls the renderer globally, so it affects all primitives
// or controls the renderer directly - ie camera control or full screen effects like blurring.  
// Example:
// EndSectionDoc 

// StartSectionDoc-pt
// estado-global
// Estado global é realmente qualquer coisa que controla o
// renderizador globalmente, então ele afeta todas as primitivas ou
// controla o renderizador diretamente - p.e. controle de câmera ou
// efeitos de tela cheia como "embaçamento".
// Exemplo:
// EndSectionDoc

// StartFunctionDoc-en
// clear-engine 
// Returns: void
// Description:
// Clears the renderer, and physics system. This command should not be called directly, use clear 
// instead, as this clears a few other things, and calls clear-engine itself.
// Example:
// (clear-engine) ; woo hoo!
// EndFunctionDoc

// StartFunctionDoc-pt
// clear-engine
// Retorna: void
// Descrição:
// Limpa o renderizador, e o sistema de física. Este comando não deve
// ser chamado diretamente, use clear ao invés, já que limpa algumas
// outras coisas também, e chama clear-engine ele mesmo.
// Exemplo:
// (clear-engine) ; woo hoo!
// EndFunctionDoc

Scheme_Object *clear_engine(int argc, Scheme_Object **argv)
{
	Engine::Get()->Renderer()->Clear();
	Engine::Get()->Physics()->Clear();
	Engine::Get()->Renderer()->ClearLights();
	Engine::Get()->ClearGrabStack();
	Engine::Get()->Renderer()->UnGrab();
	Engine::Get()->GetPFuncContainer()->Clear();
	return scheme_void;
}

// StartFunctionDoc-en
// blur amount-number
// Returns: void
// Description:
// Sets the full screen blur setting. Less is more, but if you set it too low it will make the
// on screen editing impossible to read, so save your script first :)
// Example:
// (blur 0.1) ; for nice trails
// EndFunctionDoc

// StartFunctionDoc-pt
// blur número-quantidade
// Retorna: void
// Descrição:
// Ajusta a opção de blur na tela inteira. Menos é mais, mas se você
// ajustar isto muito baixo vai fazer com que a edição na tela fique
// impossível de ler, então salve seus scripts primeiro :).
// Exemplo:
// (blur 0.1) ; para belos rastros
// EndFunctionDoc

Scheme_Object *blur(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
	ArgCheck("blur", "f", argc, argv);
	float blur=FloatFromScheme(argv[0]);	
	if (!blur) Engine::Get()->Renderer()->SetMotionBlur(false);
    else Engine::Get()->Renderer()->SetMotionBlur(true, blur);
	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// fog fogcolour-vector amount-number begin-number end-number
// Returns: void
// Description:
// Sets the fogging parameters to give a visual depth cue (aerial perspective in painter's jargon). 
// This can obscure the on screen editing, so keep the amount small.
// Example:
// (clear-colour (vector 0 0 1))   ; looks nice if the background matches
// (fog (vector 0 0 1) 0.01 1 100) ; blue fog
// EndFunctionDoc

// StartFunctionDoc-pt
// fog cor-nuvem-vetor número-quantidade número-ínicio númeor-final
// Retorna: void
// Descrição:
// Ajusta os paramêtros da neblina pra dar uma indicação de
// profundidade visual (perspectiva aérea no jargão de pintores). Isto
// pode obscurecer a edição na tela, então mantenha a quantidade baixa.
// Exemplo:
// (clear-colour (vector 0 0 1)) ; fica legal se o fundo de tela bate.
// (fog (vector 0 0 1) 0.01 1 100) ; neblina azul
// EndFunctionDoc

Scheme_Object *fog(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
	ArgCheck("fog", "vfff", argc, argv);
	Engine::Get()->Renderer()->SetFog(ColourFromScheme(argv[0]),
		FloatFromScheme(argv[1]),
		FloatFromScheme(argv[2]),
		FloatFromScheme(argv[3]));
	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// show-axis show-number
// Returns: void
// Description:
// Shows the worldspace origin axis used.
// Example:
// (show-axis 1)
// EndFunctionDoc

// StartFunctionDoc-pt
// show-axis número-mostrar
// Retorna: void
// Descrição:
// Mostra os eixos de origem do espaço usado; 
// Exemplo:
// (show-axis 1)
// EndFunctionDoc

Scheme_Object *show_axis(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
 	ArgCheck("show-axis", "i", argc, argv);
    Engine::Get()->Renderer()->ShowAxis(IntFromScheme(argv[0]));
    //Fluxus->ShowLocators(IntFromScheme(argv[0]));
	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// show-fps show-number
// Returns: void
// Description:
// Shows an fps count in the lower left of the screen.
// used.
// Example:
// (show-fps 1)
// EndFunctionDoc

// StartFunctionDoc-pt
// show-fps número-mostrar
// Retorna: void
// Descrição:
// Mostra uma contaem de fps na parte inferior esquerda da tela.
// Exemplo:
// (show-fps 1)
// EndFunctionDoc

Scheme_Object *show_fps(int argc, Scheme_Object **argv)
{
 	DECL_ARGV();
	ArgCheck("show-fps", "i", argc, argv);
    Engine::Get()->Renderer()->SetFPSDisplay(IntFromScheme(argv[0]));
	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// lock-camera primitiveid-number
// Returns: void
// Description:
// Locks the camera transform onto the specified primitive's transform. It's like parenting the camera
// to the object. This is the easiest way to procedurally drive the camera. Use an id number of 0 to 
// unlock the camera.
// Example:
// (clear)
// (define obj (build-cube)) ; make a cube for the camera to lock to
// 
// (push) ; make a background cube so we can tell what's happening
// (hint-wire)  
// (hint-unlit) 
// (colour (vector 0 0.4 0))
// (scale (vector -50 -50 -50))
// (build-cube)
// (pop)
// 
// (lock-camera obj) ; lock the camera to our first cube
// 
// (define (animate)
//     (grab obj)
//     (rotate (vector 1 0 0)) ; rotate the cube
//     (ungrab))
// 
// (every-frame (animate))
// EndFunctionDoc

// StartFunctionDoc-pt
// lock-camera número-id-primitiva
// Retorna: void
// Descrição:
// Trava a transformação da camera em cima da transformação do objeto
// específicado. É como parentear a câmera ao objeto. Esta é a forma
// mais fácil de dirigir a câmera proceduralmente. Use um número id de
// 0 para destravar a câmera.
// Exemplo:
// (clear)
// (define obj (build-cube)) ; make a cube for the camera to lock to
// 
// (push) ; make a background cube so we can tell what's happening
// (hint-wire)  
// (hint-unlit) 
// (colour (vector 0 0.4 0))
// (scale (vector -50 -50 -50))
// (build-cube)
// (pop)
// 
// (lock-camera obj) ; lock the camera to our first cube
// 
// (define (animate)
//     (grab obj)
//     (rotate (vector 1 0 0)) ; rotate the cube
//     (ungrab))
// 
// (every-frame (animate))
// EndFunctionDoc

Scheme_Object *lock_camera(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
 	ArgCheck("lock-camera", "i", argc, argv);
    Engine::Get()->Renderer()->GetCamera()->LockCamera(
		Engine::Get()->Renderer()->GetPrimitive(IntFromScheme(argv[0])));
	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// camera-lag amount-number
// Returns: void
// Description:
// The camera locking has an inbuilt lagging which means it will smoothly blend the movement relative
// to the primitive it's locked to.
// Example:
// (clear)
// (define obj (build-cube)) ; make a cube for the camera to lock to
// 
// (push) ; make a background cube so we can tell what's happening
// (hint-wire)
// (hint-unlit)
// (colour (vector 0 0.4 0))
// (scale (vector -50 -50 -50))
// (build-cube)
// (pop)
// 
// (lock-camera obj) ; lock the camera to our first cube
// (camera-lag 0.1)  ; set the lag amount, this will smooth out the cube jittery movement
// 
// (define (animate)
//     (grab obj)
//     (identity)
//     (translate (vector (modulo (round (inexact->exact (time))) 6) 0 0)) ; make a jittery movement
//     (ungrab))
// 
// (every-frame (animate))
// EndFunctionDoc

// StartFunctionDoc-pt
// camera-lag número-quantidade
// Retorna: void
// Descrição:
// O travamento da câmera tem um atraso construído junto o que
// significa que o movimento vai ser macio em relativo a primitiva ao
// qual ela está travada.
// Exemplo:
// (clear)
// (define obj (build-cube)) ; make a cube for the camera to lock to
// 
// (push) ; make a background cube so we can tell what's happening
// (hint-wire)
// (hint-unlit)
// (colour (vector 0 0.4 0))
// (scale (vector -50 -50 -50))
// (build-cube)
// (pop)
// 
// (lock-camera obj) ; lock the camera to our first cube
// (camera-lag 0.1)  ; set the lag amount, this will smooth out the cube jittery movement
// 
// (define (animate)
//     (grab obj)
//     (identity)
//     (translate (vector (modulo (round (inexact->exact (time))) 6) 0 0)) ; make a jittery movement
//     (ungrab))
// 
// (every-frame (animate))
// EndFunctionDoc

Scheme_Object *camera_lag(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
 	ArgCheck("camera-lag", "f", argc, argv);
    Engine::Get()->Renderer()->GetCamera()->SetCameraLag(FloatFromScheme(argv[0]));
	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// load-texture pngfilename-string
// Returns: textureid-number
// Description:
// Loads a texture from disk, converts it to a texture, and returns the id number. The texture loading
// is memory cached, so repeatedly calling this will not cause it to load again. Use force-load-texture
// if you are changing the texture while running the script. The png may be RGB or RGBA to use alpha 
// transparency.
// Example:
// (texture (load-texture "mytexture.png"))
// (build-cube) ; the cube will be texture mapped with the image
// EndFunctionDoc

// StartFunctionDoc-pt
// load-texture pngnomedoarquivo-string
// Retorna: void
// Descrição:
// Carrega uma imagem do disco, converte esta a uma textura e retorna
// o número id. O carregamento da textura se dá no cache de memória,
// então repetidamente chamar esta função não vai causar que carregue
// de novo. Use force-load-texture se você está mudando a textura
// enquanto o script estiver rodando. O png pode ser RGB ou RGBA para
// usar transparência alpha.
// Exemplo:
// (texture (load-texture "mytexture.png"))
// (build-cube) ; o cubo vai ser mapeado com a textura da imagem
// EndFunctionDoc

Scheme_Object *load_texture(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
 	ArgCheck("load-texture", "s", argc, argv);	
	int ret=Engine::Get()->Renderer()->LoadTexture(StringFromScheme(argv[0]));	
 	MZ_GC_UNREG(); 
    return scheme_make_integer_value(ret);
}

// StartFunctionDoc-en
// clear-texture-cache 
// Returns: void
// Description:
// Clears the texture cache, meaning changed textures on disk are reloaded.
// Example:
// (clear-texture-cache)
// EndFunctionDoc

// StartFunctionDoc-pt
// clear-texture-cache 
// Retorna: void
// Descrição:
// Clears the texture cache, meaning changed textures on disk are reloaded.
// Exemplo:
// (clear-texture-cache)
// EndFunctionDoc

Scheme_Object *clear_texture_cache(int argc, Scheme_Object **argv)
{
	Engine::Get()->Renderer()->ClearTextureCache();	
    return scheme_void;
}

// StartFunctionDoc-en
// frustum top-number bottom-number left-number right-number
// Returns: void
// Description:
// Sets the camera frustum, and thus the aspect ratio of the frame. 
// Example:
// (frustum -1 1 -0.75 0.75) ; default settings
// EndFunctionDoc

// StartFunctionDoc-pt
// frustum número-topo número-baixo número-esquerda número-direita
// Retorna: void
// Descrição:
// Ajusta o frustum da camera, e portanto o quociente de aspecto do
// frame. 
// Exemplo:
// (frustum -1 1 -0.75 0.75) ; definições padrão
// EndFunctionDoc

Scheme_Object *frustum(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
 	ArgCheck("frustum", "ffff", argc, argv);
	Engine::Get()->Renderer()->GetCamera()->SetFrustum(FloatFromScheme(argv[0]),
												FloatFromScheme(argv[1]),
									 		 	FloatFromScheme(argv[2]),
												FloatFromScheme(argv[3]));
 	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// clip front-number back-number
// Returns: void
// Description:
// Sets the front & back clipping planes for the camera frustum, and thus the viewing angle. 
// Change the front clipping distance to alter the perspective from telephoto to fisheye.
// Example:
// (clip 1 10000) ; default settings
// EndFunctionDoc

// StartFunctionDoc-pt
// clip número-frente número-trás
// Retorna: void
// Descrição:
// Ajusta os planos de clipagem da frente e de trás para o frustum da
// câmera, portanto o ângulo de visão. Mude a distância da frente do
// clip para alterar a perspectiva de telephoto para fisheye.
// Exemplo:
// (clip 1 10000) ; default settings
// EndFunctionDoc

Scheme_Object *clip(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
	ArgCheck("clip", "ff", argc, argv);
	Engine::Get()->Renderer()->GetCamera()->SetClip(FloatFromScheme(argv[0]),
											        FloatFromScheme(argv[1]));
 	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// ortho
// Returns: void
// Description:
// Sets orthographic projection - i.e. no perspective.
// Example:
// (ortho) 
// EndFunctionDoc

// StartFunctionDoc-pt
// ortho
// Retorna: void
// Descrição:
// Ajusta a projeção ortográfica - p.e. sem perspectiva.
// Exemplo:
// (ortho)
// EndFunctionDoc

Scheme_Object *ortho(int argc, Scheme_Object **argv)
{
	Engine::Get()->Renderer()->GetCamera()->SetOrtho(true);
    return scheme_void;
}

// StartFunctionDoc-en
// persp
// Returns: void
// Description:
// Sets perspective projection (the default) after ortho has been set.
// Example:
// (persp) 
// EndFunctionDoc

// StartFunctionDoc-pt
// persp
// Retorna: void
// Descrição:
// Ajusta a projeção como perspectiva (o padrão) depois que ortho foi
// acionada. 
// Exemplo:
// (persp)
// EndFunctionDoc

Scheme_Object *persp(int argc, Scheme_Object **argv)
{
	Engine::Get()->Renderer()->GetCamera()->SetOrtho(false);
    return scheme_void;
}

// StartFunctionDoc-en
// set-ortho-zoom amount-number
// Returns: void
// Description:
// Sets the zoom level for the orthographic projection. 
// Example:
// (set-ortho-zoom 2) 
// EndFunctionDoc

// StartFunctionDoc-pt
// set-ortho-zoom número-quantidade
// Retorna: void
// Descrição:
// Ajusta o nível de zoom para a projeção ortográfica.
// Exemplo:
// (set-ortho-zoom 2)
// EndFunctionDoc

Scheme_Object *set_ortho_zoom(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
	ArgCheck("set-ortho-zoom", "f", argc, argv);
	Engine::Get()->Renderer()->GetCamera()->SetOrthoZoom(FloatFromScheme(argv[0]));
 	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// backfacecull setting-number
// Returns: void
// Description:
// Turns backface culling on or off. Backface culling speeds up rendering by removing faces not 
// orientated towards the camera. Defaults to on, but this is not always desired, eg for double 
// sided polygons.
// Example:
// (backfacecull 0) 
// EndFunctionDoc

// StartFunctionDoc-pt
// backfacecull número-ajuste
// Retorna: void
// Descrição:
// Liga ou desliga o corte de face-traseira. Backface culling acelera
// a renderização removendo faces não orientadas em direção da
// câmera. É ligado por padrão, mas isto não é desejado sempre, eg
// para poligonos com dupla face.
// Exemplo:
// (backfacecull 0)
// EndFunctionDoc

Scheme_Object *backfacecull(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
	ArgCheck("backfacecull", "i", argc, argv);
	Engine::Get()->Renderer()->SetBackFaceCull(IntFromScheme(argv[0]));
 	MZ_GC_UNREG(); 
	return scheme_void;
}

// StartFunctionDoc-en
// clear-colour colour-vector
// Returns: void
// Description:
// Sets the colour we clear the renderer with, this forms the background colour for the scene.
// Example:
// (clear-colour (vector 1 0 0)) ; RED!!! 
// EndFunctionDoc

// StartFunctionDoc-pt
// clear-colour vetor-cor
// Retorna: void
// Descrição:
// Ajusta a cor que vai limpar o renderizador, isto forma a cor do
// fundo da cena.
// Exemplo:
// (clear-colour (vector 1 0 0)) ; RED!!! 
// EndFunctionDoc

Scheme_Object *clear_colour(int argc, Scheme_Object **argv)
{
 	DECL_ARGV();
	ArgCheck("clear-colour", "v", argc, argv);
    Engine::Get()->Renderer()->SetBGColour(ColourFromScheme(argv[0]));
 	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// clear-frame setting-number
// Returns: void
// Description:
// Sets the frame clearing on or off. 
// Example:
// (clear-frame 0) 
// EndFunctionDoc

// StartFunctionDoc-pt
// clear-frame número-ajuste
// Retorna: void
// Descrição:
// ajusta a limpeza do frame, desligado ou ligado.
// Exemplo:
// (clear-frame 0)
// EndFunctionDoc

Scheme_Object *clear_frame(int argc, Scheme_Object **argv)
{
 	DECL_ARGV();
	ArgCheck("clear-frame", "i", argc, argv);
	Engine::Get()->Renderer()->SetClearFrame(IntFromScheme(argv[0]));
 	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// clear-zbuffer setting-number
// Returns: void
// Description:
// Sets the zbuffer clearing on or off. 
// Example:
// (clear-zbuffer 0) 
// EndFunctionDoc

// StartFunctionDoc-pt
// clear-zbuffer número-ajuste
// Retorna: void
// Descrição:
// Ajusta a limpeza do zbuffer, desligado ou ligado.
// Exemplo:
// (clear-zbuffer 0)
// EndFunctionDoc

Scheme_Object *clear_zbuffer(int argc, Scheme_Object **argv)
{
 	DECL_ARGV();
	ArgCheck("clear-zbuffer", "i", argc, argv);
	Engine::Get()->Renderer()->SetClearZBuffer(IntFromScheme(argv[0]));
 	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// clear-accum setting-number
// Returns: void
// Description:
// Sets the accumulation buffer clearing on or off. 
// Example:
// (clear-accum 1) 
// EndFunctionDoc

// StartFunctionDoc-pt
// clear-accum número-ajuste
// Retorna: void
// Descrição:
// Ajusta a limpeza do buffer de acumulação, ligado ou desligado
// Exemplo:
// (clear-accum 1)
// EndFunctionDoc

Scheme_Object *clear_accum(int argc, Scheme_Object **argv)
{
 	DECL_ARGV();
	ArgCheck("clear-accum", "i", argc, argv);
	Engine::Get()->Renderer()->SetClearAccum(IntFromScheme(argv[0]));
 	MZ_GC_UNREG(); 
    return scheme_void;
}

// StartFunctionDoc-en
// get-camera
// Returns: matrix-vector
// Description:
// Gets the current camera transform matrix. This is the low level function, 
// use get-camera-transform instead.
// Example:
// (get-camera) 
// EndFunctionDoc

// StartFunctionDoc-pt
// get-camera
// Retorna: vetor-matriz
// Descrição:
// Pega a transformação da camera. Esta é a função de baixo nível, use
// get-camera-transform ao invés.
// Exemplo:
// (get-camera)
// EndFunctionDoc

Scheme_Object *get_camera(int argc, Scheme_Object **argv)
{
	return FloatsToScheme(Engine::Get()->Renderer()->GetCamera()->GetMatrix()->inverse().arr(),16);
}

// StartFunctionDoc-en
// get-locked-matrix
// Returns: matrix-vector
// Description:
// Gets the current camera lock transform matrix. Takes the lag into account
// Example:
// (get-locked-matrix) 
// EndFunctionDoc

// StartFunctionDoc-pt
// get-locked-matrix
// Retorna: vetor-matriz
// Descrição:
// Pega a matriz de tranformação da câmera travada. Leva em
// consideração o atraso.
// Exemplo:
// (get-locked-matrix)
// EndFunctionDoc

Scheme_Object *get_locked_matrix(int argc, Scheme_Object **argv)
{
	return FloatsToScheme(Engine::Get()->Renderer()->GetCamera()->GetLockedMatrix()->inverse().arr(),16);
}

// StartFunctionDoc-en
// set-camera
// Returns: void
// Description:
// Sets the camera transform matrix. This is the low level interface used by set-camera-transform, 
// which you should generally use instead.
// Example:
// (set-camera) 
// EndFunctionDoc

// StartFunctionDoc-pt
// set-camera 
// Retorna: void
// Descrição:
// Ajusta a matriz de transformação da câmera. Esta é a interface de
// baixo nível usada por set-camera-transform, a qual você devia usar
// geralmente ao invés.
// Exemplo:
// (set-camera)
// EndFunctionDoc

Scheme_Object *set_camera(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
	ArgCheck("set-camera", "m", argc, argv);
	dMatrix m;
	FloatsFromScheme(argv[0],m.arr(),16);
	Engine::Get()->Renderer()->GetCamera()->SetMatrix(m);
 	MZ_GC_UNREG(); 
	return scheme_void;
}

// StartFunctionDoc-en
// get-projection-transform
// Returns: projection-matrix
// Description:
// Gets the current projection matrix.
// Example:
// (get-projection-transform) 
// EndFunctionDoc

// StartFunctionDoc-pt
// get-projection-transform
// Retorna: matriz-de-projeção
// Descrição:
// Pega a matriz de projeção atual.
// Exemplo:
// (get-projection-transform)
// EndFunctionDoc

Scheme_Object *get_projection_transform(int argc, Scheme_Object **argv)
{
	return FloatsToScheme(Engine::Get()->Renderer()->GetCamera()->GetProjection().arr(),16);
}

// StartFunctionDoc-en
// get-screen-size
// Returns: size-vector
// Description:
// Returns a vector containing the current width and height of the window.
// Example:
// (get-screen-size) 
// EndFunctionDoc

// StartFunctionDoc-pt
// get-screen-size
// Retorna: vetor-tamanho
// Descrição:
// Retorna um vetor contendo a atual largura e altura da janela
// Exemplo:
// (get-screen-size)
// EndFunctionDoc

Scheme_Object *get_screen_size(int argc, Scheme_Object **argv)
{
	float res[2];
	int x=0,y=0;
	Engine::Get()->Renderer()->GetResolution(x,y);
	res[0]=x; res[1]=y;
	return FloatsToScheme(res,2);
}

// StartFunctionDoc-en
// set-screen-size size-vector
// Returns: void
// Description:
// Sets the window width and height.
// Example:
// (set-screen-size (vector 10 10)) ; small window time :) 
// EndFunctionDoc

// StartFunctionDoc-pt
// set-screen-size vetor-tamanho
// Retorna: void
// Descrição:
// Ajusta a altura e largura da janela.
// Exemplo:
// (set-screen-size (vector 10 10)) ; small window time :)
// EndFunctionDoc

Scheme_Object *set_screen_size(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
	if (!SCHEME_VECTORP(argv[0])) scheme_wrong_type("set-screen-size", "vector", 0, argc, argv);
	if (SCHEME_VEC_SIZE(argv[0])!=2) scheme_wrong_type("set-screen-size", "vector size 2", 0, argc, argv);
	float v[2];
	FloatsFromScheme(argv[0],v,2);
	// hmmm, seems a bit wrong, but hey...
	glutReshapeWindow((int)v[0],(int)v[1]);
 	MZ_GC_UNREG(); 
	return scheme_void;
}

// StartFunctionDoc-en
// select screenxpos-number screenypos-number pixelssize-number
// Returns: primitiveid-number
// Description:
// Looks in the region specified and returns the id of the closest primitive to the camera rendered 
// there, or 0 if none exist.  
// Example:
// (display (select 10 10 2))(newline)
// EndFunctionDoc

// StartFunctionDoc-pt
// select número-janelaposX número-janelaposY número-tamanho-pixel 
// Retorna: número-id-primitiva
// Descrição:
// Olha na região específicada e retorna a id da primitiva mais
// próxima à renderização da câmera lá, ou 0 se não existente.
// Exemplo:
// (display (select 10 10 2))(newline)
// EndFunctionDoc

Scheme_Object *select(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
	ArgCheck("select", "iii", argc, argv);
	int x=IntFromScheme(argv[0]);
	int y=IntFromScheme(argv[1]);
	int s=IntFromScheme(argv[2]);
 	MZ_GC_UNREG(); 
	return scheme_make_integer_value(Engine::Get()->Renderer()->Select(x,y,s));
}

// StartFunctionDoc-en
// desiredfps fps-number
// Returns: void
// Description:
// Throttles the renderer so as to not take 100% cpu. This gives an upper limit on the fps rate, which
// doesn't quite match the given number, but I'm working on it...  
// Example:
// (desiredfps 100000) ; makes fluxus render as fast as it can, and take 100% cpu.
// EndFunctionDoc 

// StartFunctionDoc-pt
// desiredfps número-fps
// Retorna: void
// Descrição:
// Desacelera o renderizador de forma a não pegar 100% de cpu. Isto dá
// um limite acima na taxa de fps, o que não completamente bate o
// número dado, mas nós estamos trabalhando nisto...
// Exemplo:
// (desiredfps 100000) ; faz fluxus renderizar tão rápido quanto pode
//                     ; e levar 100% de cpu.
// EndFunctionDoc

Scheme_Object *desiredfps(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
	ArgCheck("desiredfps", "f", argc, argv);
	Engine::Get()->Renderer()->SetDesiredFPS(scheme_real_to_double(argv[0]));
 	MZ_GC_UNREG(); 
	return scheme_void;
}

// StartFunctionDoc-en
// draw-buffer buffer_name
// Returns: void
// Description:
// Select which buffer to draw in
// for stereo mode you'd do 'back-right and 'back-left
// Example:
// (draw-buffer 'back)
// EndFunctionDoc

// StartFunctionDoc-pt
// draw-buffer nome-buffer
// Retorna: void
// Descrição:
// Seleciona qual buffer para desenhar, se em modo estéreo você iria
// fazer 'back-right e 'back-left
// Exemplo:
// (draw-buffer 'back)
// EndFunctionDoc

Scheme_Object *draw_buffer(int argc, Scheme_Object **argv)
{
  DECL_ARGV();
  ArgCheck("draw-buffer", "S", argc, argv);   

  if(IsSymbol(argv[0], "back"))
	  Engine::Get()->Renderer()->DrawBuffer(GL_BACK);
  else if(IsSymbol(argv[0],"back-right"))
	  Engine::Get()->Renderer()->DrawBuffer(GL_BACK_RIGHT);
  else if(IsSymbol(argv[0],"back-left"))
	  Engine::Get()->Renderer()->DrawBuffer(GL_BACK_LEFT);
  else if(IsSymbol(argv[0], "front"))
	  Engine::Get()->Renderer()->DrawBuffer(GL_FRONT);
  else if(IsSymbol(argv[0], "front-right"))
	  Engine::Get()->Renderer()->DrawBuffer(GL_FRONT_RIGHT);
  else if(IsSymbol(argv[0], "front-left"))
	  Engine::Get()->Renderer()->DrawBuffer(GL_FRONT_LEFT);
  else if(IsSymbol(argv[0], "right"))
	  Engine::Get()->Renderer()->DrawBuffer(GL_RIGHT);
  else if(IsSymbol(argv[0], "left"))
	  Engine::Get()->Renderer()->DrawBuffer(GL_LEFT);
  else if(IsSymbol(argv[0], "front-and-back"))
	  Engine::Get()->Renderer()->DrawBuffer(GL_FRONT_AND_BACK);
  else if(IsSymbol(argv[0], "none"))
	  Engine::Get()->Renderer()->DrawBuffer(GL_NONE);
  else {
	  //XXX should indicate an error
  }

  MZ_GC_UNREG(); 
	return scheme_void;
}

// StartFunctionDoc-en
// read-buffer buffer_name
// Returns: void
// Description:
// Select which buffer to read from
// Example:
// (read-buffer 'back)
// EndFunctionDoc

// StartFunctionDoc-pt
// read-buffer nome-buffer
// Retorna: void
// Descrição:
// Seleciona qual buffer para ler.
// Exemplo:
// (read-buffer 'back)
// EndFunctionDoc

Scheme_Object *read_buffer(int argc, Scheme_Object **argv)
{
  DECL_ARGV();
  ArgCheck("read-buffer", "S", argc, argv);   

  if(IsSymbol(argv[0], "back"))
	  Engine::Get()->Renderer()->ReadBuffer(GL_BACK);
  else if(IsSymbol(argv[0], "back-right"))
	  Engine::Get()->Renderer()->ReadBuffer(GL_BACK_RIGHT);
  else if(IsSymbol(argv[0], "back-left"))
	  Engine::Get()->Renderer()->ReadBuffer(GL_BACK_LEFT);
  else if(IsSymbol(argv[0], "front"))
	  Engine::Get()->Renderer()->ReadBuffer(GL_FRONT);
  else if(IsSymbol(argv[0], "front-right"))
	  Engine::Get()->Renderer()->ReadBuffer(GL_FRONT_RIGHT);
  else if(IsSymbol(argv[0], "front-left"))
	  Engine::Get()->Renderer()->ReadBuffer(GL_FRONT_LEFT);
  else if(IsSymbol(argv[0], "right"))
	  Engine::Get()->Renderer()->ReadBuffer(GL_RIGHT);
  else if(IsSymbol(argv[0], "left"))
	  Engine::Get()->Renderer()->ReadBuffer(GL_LEFT);
  else if(IsSymbol(argv[0], "front-and-back"))
	  Engine::Get()->Renderer()->ReadBuffer(GL_FRONT_AND_BACK);
  else if(IsSymbol(argv[0], "none"))
	  Engine::Get()->Renderer()->ReadBuffer(GL_NONE);
  else {
	  //XXX should indicate an error
  }

  MZ_GC_UNREG(); 
	return scheme_void;
}

// StartFunctionDoc-en
// set-stereo-mode mode
// Returns: bool
// Description:
// select which stereo mode to use
// currently only 'crystal-eyes and 'no-stereo are supported
// the return indicates if the operation was successful or not
// 'crystal-eyes will return false if you don't have a stereo window
// Example:
// (set-stereo-mode 'crystal-eyes)
// EndFunctionDoc

// StartFunctionDoc-pt
// set-stereo-mode modo
// Retorna: bool
// Descrição:
// seleciona qual modo estéreo a usar, atualmente somente 
// 'cristal-eyes e 'no-stereo são suportados o retorno indica se a
// operação foi bem sucedida ou não 'crystal-eyes vai retornar falso
// se você não tem uma janela estéreo.
// Exemplo:
// (set-stereo-mode 'crystal-eyes)
// EndFunctionDoc

Scheme_Object *set_stereo_mode(int argc, Scheme_Object **argv)
{
  bool success;
  DECL_ARGV();
  ArgCheck("set-stereo-mode", "S", argc, argv);   
  if(IsSymbol(argv[0], "crystal-eyes"))
	  success = Engine::Get()->Renderer()->SetStereoMode(Renderer::crystalEyes);  
  else if(IsSymbol(argv[0], "colour"))
	  success = Engine::Get()->Renderer()->SetStereoMode(Renderer::colourStereo); 
  else if(IsSymbol(argv[0], "no-stereo"))
	  success = Engine::Get()->Renderer()->SetStereoMode(Renderer::noStereo); 
  else {
	  Engine::Get()->Renderer()->SetStereoMode(Renderer::noStereo);   
	  success = false;
  }
  MZ_GC_UNREG(); 

  if(success)
	  return scheme_true;
  else
	  return scheme_false;
}

Scheme_Object *get_stereo_mode(int argc, Scheme_Object **argv)
{
  Renderer::stereo_mode_t mode;
  DECL_ARGV();
  mode = Engine::Get()->Renderer()->GetStereoMode();
  MZ_GC_UNREG(); 

  switch(mode){
		case Renderer::noStereo:
			return scheme_intern_symbol("no-stereo");
		case Renderer::crystalEyes:
			return scheme_intern_symbol("crystal-eyes");
		case Renderer::colourStereo:
			return scheme_intern_symbol("colour");	  
		default:
			return scheme_intern_symbol("no_stereo");
  }
}

// StartFunctionDoc-en
// set-colour-mask vector
// Returns: void
// Description:
// sets the colour mask
// give it a quat of booleans which correspond to the 
// red, green, blue and alpha channels respectively
// after this operation you'll only see those colour which you
// set to true (this is useful for stereo with red-blue glasses)
// Example:
// (set-colour-mask #(#t #f #f #t))
// EndFunctionDoc

// StartFunctionDoc-pt
// set-colour-mask vetor
// Retorna: void
// Descrição:
// Ajusta a máscara de cor dando a esta um quatérnio de booleanos que
// correspondem aos canais vermelho, verde, azul e alpha
// respectivamente depois desta operação você vai ver apenas aquelas
// cores que você ajustar como verdadeiras (isto é útil apenas para
// estéreo com óculos azul-vermelhos)
// Exemplo:
// (set-colour-mask #(#t #f #f #t))
// EndFunctionDoc

Scheme_Object *set_colour_mask(int argc, Scheme_Object **argv)
{
  bool rgba[4];
  DECL_ARGV();
  ArgCheck("set-colour-mask", "q", argc, argv);   
  for(unsigned int n = 0; n < 4; n++){
	  if(!SCHEME_BOOLP(SCHEME_VEC_ELS(argv[0])[n]))
		  scheme_wrong_type("set-colour-mask", "quat of booleans", 0, argc, argv);
	  rgba[n] = SCHEME_TRUEP(SCHEME_VEC_ELS(argv[0])[n]) ? true : false;
  }
  Engine::Get()->Renderer()->SetColourMask(rgba[0],rgba[1],rgba[2],rgba[3]);
  MZ_GC_UNREG(); 

  return scheme_void;
}

// StartFunctionDoc-en
// shadow-light number-setting
// Returns: void
// Description:
// Sets the light to use for generating shadows, set to 0 to disable shadow
// rendering.
// Example:
// (shadow-light 1) 
// EndFunctionDoc 

// StartFunctionDoc-pt
// shadow-light número-ajuste
// Retorna: void
// Descrição:
// Ajusta a luz para usar na geração de sombras, ajuste para 0 para
// desativar renderização de sombras.
// Exemplo:
// (shadow-light 1)
// EndFunctionDoc

Scheme_Object *shadow_light(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
	ArgCheck("shadow-light", "i", argc, argv);
	Engine::Get()->Renderer()->ShadowLight(IntFromScheme(argv[0]));
 	MZ_GC_UNREG(); 
	return scheme_void;
}

// StartFunctionDoc-en
// shadow-length number-setting
// Returns: void
// Description:
// Sets the length of the shadow volume
// rendering.
// Example:
// (shadow-length 10) 
// EndFunctionDoc 

// StartFunctionDoc-pt
// shadow-length número-ajuste
// Retorna: void
// Descrição:
// Ajusta o alcance do volume da renderização da sombra.
// Exemplo:
// (shadow-length 10)
// EndFunctionDoc

Scheme_Object *shadow_length(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
	ArgCheck("shadow-length", "f", argc, argv);
	Engine::Get()->Renderer()->ShadowLength(FloatFromScheme(argv[0]));
 	MZ_GC_UNREG(); 
	return scheme_void;
}

// StartFunctionDoc-en
// shadow-debug number-setting
// Returns: void
// Description:
// Turns on debug rendering of the shadow volume
// rendering.
// Example:
// (shadow-debug 1) 
// EndFunctionDoc 

// StartFunctionDoc-pt
// shadow-debug número-ajuste
// Retorna: void
// Descrição:
// Liga debug na renderização do volume da sombra.
// Exemplo:
// (shadow-debug 1)
// EndFunctionDoc

Scheme_Object *shadow_debug(int argc, Scheme_Object **argv)
{
	DECL_ARGV();
	ArgCheck("shadow-debug", "i", argc, argv);
	Engine::Get()->Renderer()->DebugShadows(IntFromScheme(argv[0]));
 	MZ_GC_UNREG(); 
	return scheme_void;
}

// StartFunctionDoc-en
// accum mode-symbol value-number
// Returns: void
// Description:
// Controls the accumulation buffer (just calls glAccum under the hood).
// Possible symbols are: accum load return add mult
// Example:
// (accum 'add 1) 
// EndFunctionDoc 

// StartFunctionDoc-pt
// accum simbolo-modo número-valor
// Retorna: void
// Descrição:
// Controla o buffer de acumulação (somente chama glAccum embaixo do
// tapete).
// Símbolos possíveis são: accum load return add mult
// Exemplo:
// (accum 'add 1)
// EndFunctionDoc

Scheme_Object *accum(int argc, Scheme_Object **argv)
{
  DECL_ARGV();
  ArgCheck("accum", "Sf", argc, argv);   

  if(SAME_OBJ(argv[0], scheme_intern_symbol("accum")))
	  Engine::Get()->Renderer()->Accum(GL_ACCUM,FloatFromScheme(argv[1]));
  else if(SAME_OBJ(argv[0], scheme_intern_symbol("load")))
	  Engine::Get()->Renderer()->Accum(GL_LOAD,FloatFromScheme(argv[1]));
  else if(SAME_OBJ(argv[0], scheme_intern_symbol("return")))
	  Engine::Get()->Renderer()->Accum(GL_RETURN,FloatFromScheme(argv[1]));
  else if(SAME_OBJ(argv[0], scheme_intern_symbol("add")))
	  Engine::Get()->Renderer()->Accum(GL_ADD,FloatFromScheme(argv[1]));
  else if(SAME_OBJ(argv[0], scheme_intern_symbol("mult")))
	  Engine::Get()->Renderer()->Accum(GL_MULT,FloatFromScheme(argv[1]));
  else {
	  //XXX should indicate an error
  }

  MZ_GC_UNREG(); 
  return scheme_void;
}

void GlobalStateFunctions::AddGlobals(Scheme_Env *env)
{	
	MZ_GC_DECL_REG(1);
	MZ_GC_VAR_IN_REG(0, env);
	MZ_GC_REG();

	scheme_add_global("clear-engine", scheme_make_prim_w_arity(clear_engine, "clear-engine", 0, 0), env);
	scheme_add_global("blur", scheme_make_prim_w_arity(blur, "blur", 1, 1), env);
	scheme_add_global("fog", scheme_make_prim_w_arity(fog, "fog", 4, 4), env);
	scheme_add_global("show-axis", scheme_make_prim_w_arity(show_axis, "show-axis", 1, 1), env);
	scheme_add_global("show-fps", scheme_make_prim_w_arity(show_fps, "show-fps", 1, 1), env);
	scheme_add_global("lock-camera", scheme_make_prim_w_arity(lock_camera, "lock-camera", 1, 1), env);
	scheme_add_global("camera-lag", scheme_make_prim_w_arity(camera_lag, "camera-lag", 1, 1), env);
	scheme_add_global("load-texture", scheme_make_prim_w_arity(load_texture, "load-texture", 1, 1), env);
	scheme_add_global("clear-texture-cache", scheme_make_prim_w_arity(clear_texture_cache, "clear-texture-cache", 0, 0), env);
	scheme_add_global("frustum", scheme_make_prim_w_arity(frustum, "frustum", 0, 0), env);
	scheme_add_global("clip", scheme_make_prim_w_arity(clip, "clip", 2, 2), env);
	scheme_add_global("ortho", scheme_make_prim_w_arity(ortho, "ortho", 0, 0), env);
	scheme_add_global("persp", scheme_make_prim_w_arity(persp, "persp", 0, 0), env);
	scheme_add_global("set-ortho-zoom", scheme_make_prim_w_arity(set_ortho_zoom, "set-ortho-zoom", 1, 1), env);
	scheme_add_global("backfacecull", scheme_make_prim_w_arity(backfacecull, "backfacecull", 1, 1), env);
	scheme_add_global("clear-colour", scheme_make_prim_w_arity(clear_colour, "clear-colour", 1, 1), env);
	scheme_add_global("clear-frame", scheme_make_prim_w_arity(clear_frame, "clear-frame", 1, 1), env);
	scheme_add_global("clear-zbuffer", scheme_make_prim_w_arity(clear_zbuffer, "clear-zbuffer", 1, 1), env);
	scheme_add_global("clear-accum", scheme_make_prim_w_arity(clear_accum, "clear-accum", 1, 1), env);
	scheme_add_global("get-locked-matrix", scheme_make_prim_w_arity(get_locked_matrix, "get-locked-matrix", 0, 0), env);
	scheme_add_global("get-camera", scheme_make_prim_w_arity(get_camera, "get-camera", 0, 0), env);
	scheme_add_global("set-camera", scheme_make_prim_w_arity(set_camera, "set-camera", 1, 1), env);
	scheme_add_global("get-projection-transform", scheme_make_prim_w_arity(get_projection_transform, "get-projection-transform", 0, 0), env);
	scheme_add_global("get-screen-size", scheme_make_prim_w_arity(get_screen_size, "get-screen-size", 0, 0), env);
	scheme_add_global("set-screen-size", scheme_make_prim_w_arity(set_screen_size, "set-screen-size", 1, 1), env);
	scheme_add_global("select", scheme_make_prim_w_arity(select, "select", 3, 3), env);
	scheme_add_global("desiredfps", scheme_make_prim_w_arity(desiredfps, "desiredfps", 1, 1), env);
 	scheme_add_global("draw-buffer", scheme_make_prim_w_arity(draw_buffer, "draw-buffer", 1, 1), env);
 	scheme_add_global("read-buffer", scheme_make_prim_w_arity(read_buffer, "read-buffer", 1, 1), env);
 	scheme_add_global("set-stereo-mode", scheme_make_prim_w_arity(set_stereo_mode, "set-stereo-mode", 1, 1), env);
 	scheme_add_global("get-stereo-mode", scheme_make_prim_w_arity(get_stereo_mode, "get-stereo-mode", 0, 0), env);
 	scheme_add_global("set-colour-mask", scheme_make_prim_w_arity(set_colour_mask, "set-colour-mask", 1, 1), env);
  	scheme_add_global("shadow-light", scheme_make_prim_w_arity(shadow_light, "shadow-light", 1, 1), env);
    scheme_add_global("shadow-length", scheme_make_prim_w_arity(shadow_length, "shadow-length", 1, 1), env);
	scheme_add_global("shadow-debug", scheme_make_prim_w_arity(shadow_debug, "shadow-ldebug", 1, 1), env);
	scheme_add_global("accum", scheme_make_prim_w_arity(accum, "accum", 2, 2), env);

 	MZ_GC_UNREG(); 
}
