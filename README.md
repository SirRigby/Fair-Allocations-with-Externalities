# Fair Allocations with Externalities

This repository contains C++ implementations of algorithms for **fair allocation of indivisible items under additive externalities**, based on:

> **Aziz, Suksompong, Sun, Walsh**  
> *Fairness Concepts for Indivisible Items with Externalities*  
> arXiv:2110.09066

---


This project was developed as part of **CS656: Algorithmic Game Theory** under **Prof. Sunil Simon**.

The goal of the project was to:
- understand fairness notions under **externalities**,
- implement constructive algorithms for **EF1** and **EFX**,
- experimentally explore reduction-based proofs (especially for 3 agents),
- and verify correctness via exhaustive search on reduced instances.

---


We are given:
- a set of **indivisible items**,
- a set of **agents**,
- and **additive valuations with externalities**.

Unlike standard fair division, an agent derives value not only from items they receive, but also from **who receives other items**.

Formally:

$$
V_i(\pi) = \sum_{a \in A} V_i(\pi(a), a)
$$

Here:
- $\pi(a)$ is the agent receiving item $a$
- $V_i(j,a)$ is the value agent $i$ gets when item $a$ is assigned to agent $j$

---

## Fairness Concepts

### Envy-Freeness (EF)
No agent prefers swapping bundles.

### EF1 (Envy-Free up to One Item)
Envy can be eliminated by removing **one item**.

### EFX (Envy-Free up to Any Item)
For any item whose removal reduces envy, removing it must eliminate envy.

---
