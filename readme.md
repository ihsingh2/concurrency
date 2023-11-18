## General Methodology

- We create separate producer and consumer threads, representing baristas/machines and customers respectively. 

- Since their operations are constrained based on a time series, we use a cyclic reusable barrier to synchronise their operations at every time instant, say a second.

- To be more precise, we do this more often than in a second. This is done for the sake of simplicity of implmentation and allow the producer to virtually use the entire second and exhaust the customer tolerance completely.

- We model the execution in form of a state diagram, represented using switch case here.
