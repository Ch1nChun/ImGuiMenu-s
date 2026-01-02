# Компилятор
CXX = clang++

# Имя исполняемого файла
EXE = imgui.exe
IMGUI_DIR = ../..
MAIN_DIR = ../../../..

# Поиск исходных файлов
MAIN_SOURCES = $(wildcard $(MAIN_DIR)/framework/*.cpp)
ELEMENTS_SOURCES = $(wildcard $(MAIN_DIR)/framework/widgets/*.cpp)
HELPERS_SOURCES = $(wildcard $(MAIN_DIR)/framework/helpers/*.cpp)

# Основные исходные файлы
SOURCES = main.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp
SOURCES += $(IMGUI_DIR)/imgui_draw.cpp
SOURCES += $(IMGUI_DIR)/imgui_tables.cpp
SOURCES += $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/imgui_freetype.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_win32.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_dx11.cpp

# Добавляем файлы из основных директорий
SOURCES += $(MAIN_SOURCES)
SOURCES += $(ELEMENTS_SOURCES)
SOURCES += $(HELPERS_SOURCES)

# Генерация имен объектных файлов и зависимостей
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
DEPS = $(OBJS:.o=.d)

# Проверка ОС
ifneq ($(OS),Windows_NT)
    $(error Этот Makefile предназначен только для Windows)
endif

# Флаги компиляции
CXXFLAGS = -std=c++20
CXXFLAGS += -I$(IMGUI_DIR)
CXXFLAGS += -I$(IMGUI_DIR)/backends
CXXFLAGS += -I$(MAIN_DIR)/framework
CXXFLAGS += -I$(MAIN_DIR)/framework/widgets
CXXFLAGS += -I$(MAIN_DIR)/framework/helpers
CXXFLAGS += -I$(MAIN_DIR)/thirdparty/freetype/include
CXXFLAGS += -I$(MAIN_DIR)/thirdparty/dxsdk/include
CXXFLAGS += -I$(MAIN_DIR)/thirdparty/imgui
CXXFLAGS += -DUNICODE -D_UNICODE -DWIN32 -D_WINDOWS
CXXFLAGS += -g
CXXFLAGS += -w  # Отключаем все предупреждения

# Оптимизация для Windows 11
CXXFLAGS += -march=native -mtune=native
CXXFLAGS += -D_WIN32_WINNT=0x0A00  # Windows 10/11

# Пути к библиотекам
FREETYPE_LIB_PATH = $(MAIN_DIR)/thirdparty/freetype/win64
DXSDK_LIB_PATH = $(MAIN_DIR)/thirdparty/dxsdk/lib/x64

# Библиотеки для линковки
LIBS = -L$(FREETYPE_LIB_PATH) -lfreetype
LIBS += -L$(DXSDK_LIB_PATH)
LIBS += -ld3d11 -ld3dcompiler -ldxgi -ldxguid
LIBS += -luser32 -lgdi32 -limm32 -lole32 -loleaut32 -luuid -lversion -lwinmm
LIBS += -lsetupapi -ladvapi32 -lshell32

# Проверка наличия необходимых библиотек
ifeq (,$(wildcard $(FREETYPE_LIB_PATH)/freetype.lib))
    $(error freetype.lib не найден в $(FREETYPE_LIB_PATH). Соберите freetype сначала)
endif

# Правило для запуска приложения после сборки
RUN_AFTER_BUILD = 1

# Правила для генерации зависимостей
%.d: %.cpp
	@$(CXX) $(CXXFLAGS) -MM $< -MT $(@:.d=.o) > $@

%.d: $(IMGUI_DIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -MM $< -MT $(@:.d=.o) > $@

%.d: $(IMGUI_DIR)/backends/%.cpp
	@$(CXX) $(CXXFLAGS) -MM $< -MT $(@:.d=.o) > $@

%.d: $(MAIN_DIR)/framework/%.cpp
	@$(CXX) $(CXXFLAGS) -MM $< -MT $(@:.d=.o) > $@

%.d: $(MAIN_DIR)/framework/widgets/%.cpp
	@$(CXX) $(CXXFLAGS) -MM $< -MT $(@:.d=.o) > $@

%.d: $(MAIN_DIR)/framework/helpers/%.cpp
	@$(CXX) $(CXXFLAGS) -MM $< -MT $(@:.d=.o) > $@

# Правила компиляции
%.o: %.cpp
	@echo Компиляция $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(IMGUI_DIR)/%.cpp
	@echo Компиляция $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(IMGUI_DIR)/backends/%.cpp
	@echo Компиляция $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(MAIN_DIR)/framework/%.cpp
	@echo Компиляция $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(MAIN_DIR)/framework/widgets/%.cpp
	@echo Компиляция $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(MAIN_DIR)/framework/helpers/%.cpp
	@echo Компиляция $<
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

# Основная цель
all: $(EXE)
ifeq ($(RUN_AFTER_BUILD),1)
	@echo Запуск приложения...
	@./$(EXE)
endif

$(EXE): $(OBJS)
	@echo Линковка...
	@$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)
	@echo Сборка завершена для Windows 11

# Включение зависимостей
-include $(DEPS)

# Очистка
clean:
	@echo Очистка...
	@del /Q $(EXE) 2>nul || true
	@del /Q *.o 2>nul || true
	@del /Q *.d 2>nul || true
	@echo Очистка завершена

# Помощь
help:
	@echo Доступные цели:
	@echo   all     - собрать и запустить приложение
	@echo   build   - только собрать приложение
	@echo   clean   - очистить сборочные файлы
	@echo   help    - показать эту справку

# Альтернативная цель только для сборки
build: RUN_AFTER_BUILD = 0
build: $(EXE)

.PHONY: all clean help build