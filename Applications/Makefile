APPS_DIR := .

CC := gcc
CFLAGS := -Wall -Wextra -O2 -fPIC -I../include
LDFLAGS := -L../build/output

APPS := ActivityMonitor Calculator Calendar Chrome CodeEditor Contacts \
		DiskUtility FileManager Finder Mail Maps MediaPlayer MusicPlayer \
		Notes PDFReader Photos Presentation Reminders RemoteDesktop Safari \
		Screenshot Settings Spreadsheet SystemCleaner SystemPreferences \
		Terminal VPNManager Weather WordProcessor PackageManager

.PHONY: all clean $(APPS)

all: $(APPS)

$(APPS):
	@echo "Building $@.app..."
	$(MAKE) -C $@.app

clean:
	@for app in $(APPS); do \
		$(MAKE) -C $$app.app clean; \
	done
