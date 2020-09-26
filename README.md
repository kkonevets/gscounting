# DRAFT (work in progress)
Given an arbitrary graph with node labels the task is to propagate these labels to unlabeled nodes. Sometimes you have no information other than neighbor labels.
Classical algorithms, like Label Propagation, solve this task in a deterministic way. Here a probabilistic aproach is taken by using a GraphSage algorithm with neighbor label counts as node features. It is also possible to set multiple labels for a node via multilable classification.