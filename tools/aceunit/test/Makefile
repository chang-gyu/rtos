DIRS= \
     basic \
     basicEmbedded \
     bugs/callingAfterClass \
     comment \
     cplusplus \
     cplusplus2 \
     exhand \
     explicitMsg \
     group \
     longjmp \
     loop \
     recursive \
     twoFixtures \
     xmlLog \
#     printf \
#     KeilTest \

.PHONY : all
all: $(DIRS)

.PHONY : clean
clean: $(DIRS)

.PHONY : lint
lint: $(DIRS)

.PHONY : $(DIRS)
$(DIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
