#!/bin/bash

set -ev

echo "Deployment init"

echo "$DOCKER_PASSWORD" | docker login -u "$DOCKER_USERNAME" --password-stdin
docker tag pdf2invoice pdf2cash/pdf2invoice:latest
docker push pdf2cash/pdf2invoice:latest








