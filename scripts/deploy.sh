#!/bin/bash

set -ev

echo "Deployment init"

pip install awscli
export PATH=$PATH:$HOME/.local/bin
echo $(aws ecr get-authorization-token --region us-east-2 --output text --query 'authorizationData[].authorizationToken' | base64 -d | cut -d: -f2) | docker login -u AWS https://742466423033.dkr.ecr.us-east-2.amazonaws.com --password-stdin
docker tag pdf2invoice:latest 742466423033.dkr.ecr.us-east-2.amazonaws.com/pdf2cash:pdf2invoice
docker push 742466423033.dkr.ecr.us-east-2.amazonaws.com/pdf2cash:pdf2invoice









