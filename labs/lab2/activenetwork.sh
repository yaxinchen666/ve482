#!/bin/bash

ifconfig | awk '/inet /' | awk '{print $2}'



