#-------------------------------------------------
#
# Project created by QtCreator 2012-04-29T22:23:49
#
#-------------------------------------------------

QT += core gui

TARGET = cheetah-texture-packer

QT_VERSION=$$[QT_VERSION]

contains(QT_VERSION, "^5.*") {
  QT += widgets
} else {
}

TEMPLATE = app
INCLUDEPATH += src \
	../../../Libraries/fx-gltf/src \
	../../../Libraries/fx-gltf/test/thirdparty \
	../../../Libraries/basis_universal/

LIBS += -lGLEW -lGL -lGLU -ldrm

CONFIG += c++14

DEFINES += GLM_EXT_INCLUDED \"_gl=gl->\"

SOURCES += src/main.cpp\
	../../../Libraries/basis_universal/apg_bmp.c \
	../../../Libraries/basis_universal/basisu_astc_decomp.cpp \
	../../../Libraries/basis_universal/basisu_backend.cpp \
	../../../Libraries/basis_universal/basisu_basis_file.cpp \
	../../../Libraries/basis_universal/basisu_bc7enc.cpp \
	../../../Libraries/basis_universal/basisu_comp.cpp \
	../../../Libraries/basis_universal/basisu_enc.cpp \
	../../../Libraries/basis_universal/basisu_etc.cpp \
	../../../Libraries/basis_universal/basisu_frontend.cpp \
	../../../Libraries/basis_universal/basisu_global_selector_palette_helpers.cpp \
	../../../Libraries/basis_universal/basisu_gpu_texture.cpp \
	../../../Libraries/basis_universal/basisu_pvrtc1_4.cpp \
	../../../Libraries/basis_universal/basisu_resample_filters.cpp \
	../../../Libraries/basis_universal/basisu_resampler.cpp \
	../../../Libraries/basis_universal/basisu_ssim.cpp \
	../../../Libraries/basis_universal/basisu_uastc_enc.cpp \
	../../../Libraries/basis_universal/jpgd.cpp \
	../../../Libraries/basis_universal/lodepng.cpp \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder.cpp \
	../../../Libraries/fx-gltf/src/bufferinfo.cpp \
	../../../Libraries/fx-gltf/src/fx/gltf.cpp \
	../../../Libraries/fx-gltf/src/gltf_stl_accessor.cpp \
	src/Shaders/colorshader.cpp \
	src/Shaders/defaulttextures.cpp \
	src/Shaders/defaultvaos.cpp \
	src/Shaders/glprogram.cpp \
	src/Shaders/gltfmetallicroughness.cpp \
	src/Shaders/spriteshaderbase.cpp \
	src/Shaders/transparencyshader.cpp \
	src/Shaders/unlitshader.cpp \
	src/Shaders/velvetshader.cpp \
	src/Sprite/animation.cpp \
	src/Sprite/document.cpp \
	src/Sprite/image.cpp \
	src/Sprite/material.cpp \
	src/Sprite/object.cpp \
	src/Sprite/spritesheet.cpp \
	src/Support/counted_string.cpp \
	src/Support/imagesupport.cpp \
	src/commandlist.cpp \
	src/imagemetadata.cpp \
	src/lf_math.cpp \
        src/mainwindow.cpp \
	src/packersettings.cpp \
	src/preferences.cpp \
	src/widgets/glviewwidget.cpp \
	src/widgets/spritemodel.cpp \
        src/imagepacker.cpp \
        src/imagecrop.cpp \
        src/maxrects.cpp \
    src/support.cpp \
    src/parsearguments.cpp \
    src/widgets/qclickablelabel.cpp \
    src/settingspanel.cpp \
    src/superxbrscaler.cpp \
    src/rc_crc32.c

HEADERS  += src/mainwindow.h \
	../../../Libraries/basis_universal/apg_bmp.h \
	../../../Libraries/basis_universal/basisu_astc_decomp.h \
	../../../Libraries/basis_universal/basisu_backend.h \
	../../../Libraries/basis_universal/basisu_basis_file.h \
	../../../Libraries/basis_universal/basisu_bc7enc.h \
	../../../Libraries/basis_universal/basisu_comp.h \
	../../../Libraries/basis_universal/basisu_enc.h \
	../../../Libraries/basis_universal/basisu_etc.h \
	../../../Libraries/basis_universal/basisu_frontend.h \
	../../../Libraries/basis_universal/basisu_global_selector_palette_helpers.h \
	../../../Libraries/basis_universal/basisu_gpu_texture.h \
	../../../Libraries/basis_universal/basisu_miniz.h \
	../../../Libraries/basis_universal/basisu_pvrtc1_4.h \
	../../../Libraries/basis_universal/basisu_resampler.h \
	../../../Libraries/basis_universal/basisu_resampler_filters.h \
	../../../Libraries/basis_universal/basisu_ssim.h \
	../../../Libraries/basis_universal/basisu_uastc_enc.h \
	../../../Libraries/basis_universal/jpgd.h \
	../../../Libraries/basis_universal/lodepng.h \
	../../../Libraries/basis_universal/transcoder/basisu.h \
	../../../Libraries/basis_universal/transcoder/basisu_file_headers.h \
	../../../Libraries/basis_universal/transcoder/basisu_global_selector_cb.h \
	../../../Libraries/basis_universal/transcoder/basisu_global_selector_palette.h \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder.h \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder_internal.h \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder_uastc.h \
	../../../Libraries/fx-gltf/src/accessorreader.hpp \
	../../../Libraries/fx-gltf/src/accessortypeinfo.hpp \
	../../../Libraries/fx-gltf/src/bufferinfo.h \
	../../../Libraries/fx-gltf/src/componenttypeinfo.hpp \
	../../../Libraries/fx-gltf/src/fx/extensions/khr_materials.h \
	../../../Libraries/fx-gltf/src/fx/gltf.h \
	../../../Libraries/fx-gltf/src/fx/gltf_forward.hpp \
	../../../Libraries/fx-gltf/src/gltf_stl_accessor.h \
	src/Shaders/colorshader.h \
	src/Shaders/defaulttextures.h \
	src/Shaders/defaultvaos.h \
	src/Shaders/glprogram.h \
	src/Shaders/gltfmetallicroughness.h \
	src/Shaders/spriteshaderbase.h \
	src/Shaders/transparencyshader.h \
	src/Shaders/unlitshader.h \
	src/Shaders/velvetshader.h \
	src/Sprite/animation.h \
	src/Sprite/document.h \
	src/Sprite/image.h \
	src/Sprite/material.h \
	src/Sprite/object.h \
	src/Sprite/spritesheet.h \
	src/Support/counted_ptr.hpp \
	src/Support/counted_string.h \
	src/Support/countedsizedarray.hpp \
	src/Support/glm_iostream.hpp \
	src/Support/imagesupport.h \
	src/Support/vectoroperations.hpp \
	src/commandinterface.hpp \
	src/commandlist.h \
	src/enums.hpp \
	src/imagemetadata.h \
	src/lf_math.h \
	src/packersettings.h \
	src/widgets/glviewwidget.h \
	src/widgets/spritemodel.h \
        src/imagepacker.h \
        src/maxrects.h \
    src/support.h \
    src/parsearguments.h \
    src/widgets/qclickablelabel.h \
    src/settingspanel.h \
    src/superxbrscaler.h \
    src/widgets/qconstrainedspinbox.hpp \
    src/qimageptr.hpp \
    src/rc_crc32.h

FORMS    += src/mainwindow.ui

TRANSLATIONS += src/tile_ru.ts

DISTFILES += \
	../../../Libraries/basis_universal/LICENSE \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder_tables_astc.inc \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder_tables_astc_0_255.inc \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder_tables_atc_55.inc \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder_tables_atc_56.inc \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder_tables_bc7_m5_alpha.inc \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder_tables_bc7_m5_color.inc \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder_tables_dxt1_5.inc \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder_tables_dxt1_6.inc \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder_tables_pvrtc2_45.inc \
	../../../Libraries/basis_universal/transcoder/basisu_transcoder_tables_pvrtc2_alpha_33.inc
