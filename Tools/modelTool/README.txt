The early GTI SDK Releases use three files to describe a trained neural network, i.e., CNN model file, 
FC model file and Class label file. Starting from SDK4.0, all the trained models will be described 
by using one single GTI-defined model structure file.
  
This GTI model operation tool helps to convert the old model files into the new GTI model file, 
and to separate the new GTI model file into old model files.

As an example, the gti_gnet3.model is a GTI based VGG model with three Fully Connected Layers,
after run command “./modelTool modeldec gti_gnet3.model”,  following 6 files will be generated:

gti_gnet3.model.0		// JSON file that describes the network
gti_gnet3.model.1		// CNN model file
gti_gnet3.model.2		// FC6 model file
gti_gnet3.model.3		// FC7 model file
gti_gnet3.model.4		// FC8 model file
gti_gnet3.model.5  	// Label file with 20 classes

The command “./modelTool modelenc gti_gnet3.model.0” is then used to create a single GTI model file,
i.e., gti_gnet3.model.0.gti, based on above 6 files. This new model file can be used by SDK APIs.

All the legacy GTI models can be converted to the new GTI model format by using this tool.
